#pragma once

#include <array>
#include <atomic>

#include <dory/types.h>
#include <dory/constants.h>
#include <dory/macros/assert.h>

namespace dory::data_structures::memory_reclamation::ebr
{
    struct RetiredNode
    {
        void* ptr = nullptr;
        void (*deleter)(void*) = nullptr;
        u64 retire_epoch = 0;
    };

    template <u32 MaxRetired>
    struct RetireList
    {
        std::array<RetiredNode, MaxRetired> nodes{};
        u32 count = 0;

        [[nodiscard]] bool full() const noexcept
        {
            return count == MaxRetired;
        }

        void push(void* ptr, void (*deleter)(void*), u64 epoch) noexcept
        {
            assert::debug(count < MaxRetired, "ebr::RetireList::push: retire list is full");
            nodes[count++] = RetiredNode{ptr, deleter, epoch};
        }

        template <typename Fn>
        void reclaim_if(Fn&& can_reclaim) noexcept
        {
            u32 write = 0;
            for (u32 read = 0; read < count; ++read)
            {
                RetiredNode& n = nodes[read];
                if (can_reclaim(n))
                {
                    n.deleter(n.ptr);
                }
                else
                {
                    if (write != read)
                        nodes[write] = n;
                    ++write;
                }
            }
            count = write;
        }
    };

    struct alignas(constants::CacheLineSize) ThreadEpochState
    {
        std::atomic<u64> local_epoch{0};
        std::atomic<bool> active{false};
    };

    template <u32 MaxThreads, u32 MaxRetiredPerThread>
    class Domain
    {
    public:
        class Guard
        {
        public:
            Guard() noexcept = default;

            Guard(Domain& domain, u32 thread_index) noexcept
                : m_domain(&domain), m_thread_index(thread_index), m_entered(true)
            {
                m_domain->enter(m_thread_index);
            }

            Guard(const Guard&) = delete;
            Guard& operator=(const Guard&) = delete;

            Guard(Guard&& other) noexcept
                : m_domain(other.m_domain)
                , m_thread_index(other.m_thread_index)
                , m_entered(other.m_entered)
            {
                other.m_domain = nullptr;
                other.m_entered = false;
            }

            Guard& operator=(Guard&& other) = delete;

            ~Guard()
            {
                if (m_entered)
                    m_domain->leave(m_thread_index);
            }

        private:
            Domain* m_domain = nullptr;
            u32 m_thread_index = 0;
            bool m_entered = false;
        };

    public:
        Domain() = default;

        static constexpr u32 max_threads() noexcept { return MaxThreads; }

        Guard pin(u32 thread_index) noexcept
        {
            return Guard(*this, thread_index);
        }

        void enter(u32 thread_index) noexcept
        {
            ThreadEpochState& ts = m_threads[thread_index];

            // Publish the current epoch first, then mark active.
            const u64 epoch = m_global_epoch.load(std::memory_order_acquire);
            ts.local_epoch.store(epoch, std::memory_order_relaxed);
            ts.active.store(true, std::memory_order_release);

            // Re-read global epoch in case it advanced concurrently.
            const u64 verify = m_global_epoch.load(std::memory_order_acquire);
            if (verify != epoch)
            {
                ts.local_epoch.store(verify, std::memory_order_relaxed);
            }
        }

        void leave(u32 thread_index) noexcept
        {
            m_threads[thread_index].active.store(false, std::memory_order_release);
        }

        template <typename T>
        void retire(u32 thread_index, T* ptr) noexcept
        {
            auto deleter = [](void* p) noexcept
            {
                delete static_cast<T*>(p);
            };

            retire(thread_index, ptr, deleter);
        }

        void retire(u32 thread_index, void* ptr, void (*deleter)(void*)) noexcept
        {
            const u64 epoch = m_global_epoch.load(std::memory_order_acquire);
            RetireList<MaxRetiredPerThread>& list = m_retired[thread_index];

            if (list.full())
            {
                collect(thread_index);
                if (list.full())
                {
                    // At this point reclamation is blocked by lagging threads.
                    // In an engine you might flush later, grow capacity,
                    // or hand this to a fallback allocator queue.
                    assert::debug(false, "EBR retire list full; reclamation stalled");
                    return;
                }
            }

            list.push(ptr, deleter, epoch);

            // Opportunistic maintenance.
            if ((list.count & 7u) == 0u)
            {
                try_advance_epoch();
                collect(thread_index);
            }
        }

        void try_advance_epoch() noexcept
        {
            const u64 current = m_global_epoch.load(std::memory_order_acquire);

            for (u32 i = 0; i < MaxThreads; ++i)
            {
                const bool active = m_threads[i].active.load(std::memory_order_acquire);
                if (!active)
                    continue;

                const u64 local = m_threads[i].local_epoch.load(std::memory_order_acquire);

                // Active thread still in current or older epoch: cannot advance.
                if (local <= current)
                    return;
            }

            m_global_epoch.compare_exchange_strong(
                const_cast<u64&>(current),
                current + 1,
                std::memory_order_acq_rel,
                std::memory_order_acquire);
        }

        void collect(u32 thread_index) noexcept
        {
            const u64 global = m_global_epoch.load(std::memory_order_acquire);
            const u64 safe_epoch = (global >= 2) ? (global - 2) : 0;

            m_retired[thread_index].reclaim_if(
                [safe_epoch](const RetiredNode& n) noexcept
                {
                    return n.retire_epoch <= safe_epoch;
                });
        }

        void collect_all() noexcept
        {
            try_advance_epoch();
            for (u32 i = 0; i < MaxThreads; ++i)
                collect(i);
        }

        [[nodiscard]] u64 current_epoch() const noexcept
        {
            return m_global_epoch.load(std::memory_order_acquire);
        }

    private:
        alignas(constants::CacheLineSize) std::atomic<u64> m_global_epoch{1};
        std::array<ThreadEpochState, MaxThreads> m_threads{};
        std::array<RetireList<MaxRetiredPerThread>, MaxThreads> m_retired{};
    };
}