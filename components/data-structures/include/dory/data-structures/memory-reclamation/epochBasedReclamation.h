#pragma once

#include <array>
#include <atomic>

#include <dory/types.h>
#include <dory/constants.h>
#include <dory/macros/assert.h>

#include "janitor.h"
#include "retireList.h"
#include "guard.h"

namespace dory::data_structures::memory_reclamation::ebr
{
    struct EpochRetiredNode: public RetiredNode
    {
        u64 retireEpoch = 0;
    };

    struct alignas(constants::CacheLineSize) ThreadEpochState
    {
        std::atomic<u64> localEpoch{0};
        std::atomic<bool> active{false};
    };

    template <u32 MaxThreads, u32 MaxRetiredPerThread>
    class Domain
    {
    public:
        using RetiredNodeType = EpochRetiredNode;
        using RetireListType = RetireList<RetiredNodeType, MaxRetiredPerThread>;
        using GuardType = Guard<Domain>;

    private:
        alignas(constants::CacheLineSize) std::atomic<u64> _globalEpoch {1};
        std::array<ThreadEpochState, MaxThreads> _threads {};
        std::array<RetireListType, MaxThreads> _retired {};
        std::atomic_flag _collecting = ATOMIC_FLAG_INIT;

    public:
        Domain() = default;

        ~Domain()
        {
            drain();
        }

        static constexpr u32 max_threads() { return MaxThreads; }

        GuardType makeGuard(const ThreadId threadIndex, const PointerToken pointerToken = 0)
        {
            return GuardType(*this, threadIndex, pointerToken);
        }

        void enter(ThreadId threadIndex)
        {
            ThreadEpochState& ts = _threads[threadIndex];

            // Publish the current epoch first, then mark active.
            const u64 epoch = _globalEpoch.load(std::memory_order_acquire);
            ts.localEpoch.store(epoch, std::memory_order_relaxed);
            ts.active.store(true, std::memory_order_release);

            // Re-read global epoch in case it advanced concurrently.
            const u64 verify = _globalEpoch.load(std::memory_order_acquire);
            if (verify != epoch)
            {
                ts.localEpoch.store(verify, std::memory_order_relaxed);
            }
        }

        void leave(ThreadId threadIndex)
        {
            _threads[threadIndex].active.store(false, std::memory_order_release);
        }

        void retire(u32 thread_index, void* ptr, Janitor* janitor) noexcept
        {
            const u64 epoch = _globalEpoch.load(std::memory_order_acquire);
            RetireListType& retireList = _retired[thread_index];

            if (retireList.full())
            {
                collect(thread_index);
                if (retireList.full())
                {
                    // At this point reclamation is blocked by lagging threads.
                    // In an engine you might flush later, grow capacity,
                    // or hand this to a fallback allocator queue.
                    assert::debug(false, "EBR retire list full; reclamation stalled");
                    return;
                }
            }

            retireList.push(RetiredNodeType { ptr, janitor, epoch });

            // Opportunistic maintenance.
            if ((retireList.count & 7u) == 0u)
            {
                tryAdvanceEpoch();
                collect(thread_index);
            }
        }

        static void clearPointerSlot(const PointerToken pointerToken)
        {}

        static void occupyPointerSlot(void* ptr, const PointerToken pointerToken)
        {}

        template <typename T>
        static T* occupyAtomicPointerSlot(std::atomic<T*>& src, const PointerToken pointerToken)
        {
            return src.load(std::memory_order_acquire);
        }

        void tryAdvanceEpoch() noexcept
        {
            const u64 current = _globalEpoch.load(std::memory_order_acquire);

            for (u32 i = 0; i < MaxThreads; ++i)
            {
                const bool active = _threads[i].active.load(std::memory_order_acquire);
                if (!active)
                    continue;

                const u64 local = _threads[i].localEpoch.load(std::memory_order_acquire);

                // Active thread still in current or older epoch: cannot advance.
                if (local <= current)
                    return;
            }

            _globalEpoch.compare_exchange_strong(
                const_cast<u64&>(current),
                current + 1,
                std::memory_order_acq_rel,
                std::memory_order_acquire);
        }

        void collect(u32 thread_index) noexcept
        {
            const u64 global = _globalEpoch.load(std::memory_order_acquire);
            const u64 safe_epoch = (global >= 2) ? (global - 2) : 0;

            _retired[thread_index].reclaimIf(
                [safe_epoch](const RetiredNodeType& n) noexcept
                {
                    return n.retireEpoch <= safe_epoch;
                });
        }

        void collectAll() noexcept
        {
            //Check if some thread is doing collection currently
            if(_collecting.test_and_set(std::memory_order_acquire))
                return;

            tryAdvanceEpoch();
            for (u32 i = 0; i < MaxThreads; ++i)
                collect(i);

            _collecting.clear(std::memory_order_release);
        }

        [[nodiscard]] u64 getCurrentEpoch() const noexcept
        {
            return _globalEpoch.load(std::memory_order_acquire);
        }

        void drain()
        {
            collectAll();
        }
    };
}
