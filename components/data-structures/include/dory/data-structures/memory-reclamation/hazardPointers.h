#pragma once

#include <atomic>
#include <array>
#include <cstdint>
#include <cassert>
#include <optional>
#include <utility>
#include <algorithm>

#include <dory/types.h>
#include <dory/interface.h>
#include <dory/data-structures/function.h>

namespace dory::data_structures::memory_reclamation::hazard_pointers
{
    //TODO: move to global constants
    constexpr u32 CacheLineSize = 64;

    class Janitor: Interface
    {
    public:
        virtual void cleanup(void* ptr) = 0;
    };

    template<typename T, typename TAllocator>
    class ObjectJanitor: public Janitor
    {
    private:
        TAllocator& _allocator;

    public:
        explicit ObjectJanitor(TAllocator& allocator):
            _allocator(allocator)
        {}

        void cleanup(void* ptr) override
        {
            _allocator.deallocateObject(static_cast<T*>(ptr));
        }
    };

    struct RetiredNode
    {
        void* ptr = nullptr;
        Janitor* janitor = nullptr;
    };

    template <u32 Capacity>
    struct RetireList
    {
        std::array<RetiredNode, Capacity> nodes{};
        u32 count = 0;

        bool full() const noexcept
        {
            return count == Capacity;
        }

        void push(void* ptr, Janitor* janitor) noexcept
        {
            assert(count < Capacity);
            nodes[count++] = RetiredNode{ ptr, janitor };
        }
    };

    struct alignas(CacheLineSize) HazardSlot
    {
        std::atomic<void*> ptr{nullptr};
    };

    template <u32 MaxThreads, u32 SlotsPerThread, u32 RetireCapacity>
    class Domain
    {
    public:
        static constexpr u32 MaxHazards = MaxThreads * SlotsPerThread;

    private:
        std::array<HazardSlot, MaxHazards> _hazards {};
        std::array<RetireList<RetireCapacity>, MaxThreads> _retired {};

    public:
        class Guard
        {
        public:
            Guard() noexcept = default;

            Guard(Domain& domain, u32 slot_index) noexcept
                : m_domain(&domain), m_slot_index(slot_index)
            {
            }

            Guard(const Guard&) = delete;
            Guard& operator=(const Guard&) = delete;

            Guard(Guard&& other) noexcept
                : m_domain(other.m_domain)
                , m_slot_index(other.m_slot_index)
            {
                other.m_domain = nullptr;
            }

            Guard& operator=(Guard&&) = delete;

            ~Guard()
            {
                reset();
            }

            void reset() noexcept
            {
                if (m_domain)
                {
                    m_domain->_hazards[m_slot_index].ptr.store(nullptr, std::memory_order_release);
                    m_domain = nullptr;
                }
            }

            void protect_raw(void* p) noexcept
            {
                assert(m_domain);
                m_domain->_hazards[m_slot_index].ptr.store(p, std::memory_order_release);
            }

            template <typename T>
            T* get_protected(std::atomic<T*>& src) noexcept
            {
                assert(m_domain);

                T* p = nullptr;
                do
                {
                    p = src.load(std::memory_order_acquire);
                    m_domain->_hazards[m_slot_index].ptr.store(p, std::memory_order_release);
                }
                while (p != src.load(std::memory_order_acquire));

                return p;
            }

        private:
            Domain* m_domain = nullptr;
            u32 m_slot_index = 0;
        };

    public:
        Domain() = default;

        static constexpr u32 max_threads() noexcept { return MaxThreads; }
        static constexpr u32 slots_per_thread() noexcept { return SlotsPerThread; }

        Guard make_guard(u32 thread_index, u32 slot_in_thread = 0) noexcept
        {
            assert(thread_index < MaxThreads);
            assert(slot_in_thread < SlotsPerThread);
            return Guard(*this, hazard_index(thread_index, slot_in_thread));
        }

        /*template <typename T, typename TAllocator>
        void retire(u32 thread_index, T* ptr, TAllocator& allocator) noexcept
        {
            static ObjectJanitor<T, TAllocator> janitor { allocator };

            retire(thread_index, ptr, janitor);
        }*/

        void retire(u32 thread_index, void* ptr, Janitor* janitor) noexcept
        {
            assert(thread_index < MaxThreads);

            auto& retired = _retired[thread_index];

            if (retired.full())
            {
                scan(thread_index);

                if (retired.full())
                {
                    assert(false && "Hazard pointer retire list full");
                    return;
                }
            }

            retired.push(ptr, janitor);

            // Opportunistic batch scan.
            if ((retired.count & 15u) == 0u)
            {
                scan(thread_index);
            }
        }

        void scan(u32 thread_index) noexcept
        {
            assert(thread_index < MaxThreads);

            // Snapshot all hazard pointers into a local fixed array.
            std::array<void*, MaxHazards> snapshot{};
            u32 hazard_count = 0;

            for (u32 i = 0; i < MaxHazards; ++i)
            {
                void* p = _hazards[i].ptr.load(std::memory_order_acquire);
                if (p != nullptr)
                {
                    snapshot[hazard_count++] = p;
                }
            }

            std::sort(snapshot.begin(), snapshot.begin() + hazard_count);

            auto& retired = _retired[thread_index];

            u32 write = 0;
            for (u32 read = 0; read < retired.count; ++read)
            {
                RetiredNode& n = retired.nodes[read];

                const bool protected_now =
                    std::binary_search(snapshot.begin(), snapshot.begin() + hazard_count, n.ptr);

                if (protected_now)
                {
                    if (write != read)
                        retired.nodes[write] = n;
                    ++write;
                }
                else if(n.janitor != nullptr)
                {
                    n.janitor->cleanup(n.ptr);
                }
            }

            retired.count = write;
        }

        void scan_all() noexcept
        {
            for (u32 i = 0; i < MaxThreads; ++i)
                scan(i);
        }

        bool is_hazard(void* p) const noexcept
        {
            for (u32 i = 0; i < MaxHazards; ++i)
            {
                if (_hazards[i].ptr.load(std::memory_order_acquire) == p)
                    return true;
            }
            return false;
        }

    private:
        static constexpr u32 hazard_index(u32 thread_index, u32 slot_in_thread) noexcept
        {
            return thread_index * SlotsPerThread + slot_in_thread;
        }
    };
}