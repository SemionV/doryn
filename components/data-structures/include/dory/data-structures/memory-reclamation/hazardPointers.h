#pragma once

#include <atomic>
#include <array>
#include <algorithm>

#include <dory/types.h>
#include <dory/constants.h>
#include <dory/macros/assert.h>

#include "janitor.h"

namespace dory::data_structures::memory_reclamation::hazard_pointers
{
    using SizeType = u32;
    
    struct RetiredNode
    {
        void* ptr = nullptr;
        Janitor* janitor = nullptr;
    };

    template <SizeType Capacity>
    struct RetireList
    {
        std::array<RetiredNode, Capacity> nodes {};
        SizeType count = 0;

        [[nodiscard]] bool full() const noexcept
        {
            return count == Capacity;
        }

        void push(void* ptr, Janitor* janitor) noexcept
        {
            assert::debug(count < Capacity, "HazardPointers::RetireList::push(): Count is too large");
            nodes[count++] = RetiredNode { ptr, janitor };
        }
    };

    struct alignas(constants::CacheLineSize) HazardSlot
    {
        std::atomic<void*> ptr { nullptr };
    };

    template <SizeType MaxThreads, SizeType SlotsPerThread, SizeType RetireCapacity>
    class Domain
    {
    public:
        static constexpr SizeType MaxHazards = MaxThreads * SlotsPerThread;

    private:
        std::array<HazardSlot, MaxHazards> _hazards {};
        std::array<RetireList<RetireCapacity>, MaxThreads> _retired {};

    public:
        class Guard
        {
        private:
            Domain* _domain = nullptr;
            SizeType _slotIndex = 0;

        public:
            Guard() noexcept = default;

            Guard(Domain& domain, const SizeType slotIndex) noexcept:
                _domain(&domain),
                _slotIndex(slotIndex)
            {}

            Guard(const Guard&) = delete;
            Guard& operator=(const Guard&) = delete;

            Guard(Guard&& other) noexcept:
                _domain(other._domain),
                _slotIndex(other._slotIndex)
            {
                other._domain = nullptr;
            }

            Guard& operator=(Guard&&) = delete;

            ~Guard()
            {
                reset();
            }

            void reset() noexcept
            {
                if (_domain)
                {
                    _domain->_hazards[_slotIndex].ptr.store(nullptr, std::memory_order_release);
                    _domain = nullptr;
                }
            }

            void protectRaw(void* p) noexcept
            {
                assert::debug(_domain, "HazardPointers::Domain::Guard: Domain pointer is nullptr");
                _domain->_hazards[_slotIndex].ptr.store(p, std::memory_order_release);
            }

            template <typename T>
            T* getProtected(std::atomic<T*>& src) noexcept
            {
                assert::debug(_domain, "HazardPointers::Domain::Guard: Domain pointer is nullptr");

                T* ptr = nullptr;
                do
                {
                    ptr = src.load(std::memory_order_acquire);
                    _domain->_hazards[_slotIndex].ptr.store(ptr, std::memory_order_release);
                }
                while (ptr != src.load(std::memory_order_acquire));

                return ptr;
            }
        };

    public:
        Domain() = default;

        static constexpr SizeType getMaxThreads() noexcept { return MaxThreads; }
        static constexpr SizeType getSlotsPerThread() noexcept { return SlotsPerThread; }

        Guard makeGuard(const ThreadId threadIndex, const SizeType slotInThread = 0) noexcept
        {
            assert::debug(threadIndex < MaxThreads, "HazardPointers::Domain::makeGuard: Thread index is out of range");
            assert::debug(slotInThread < SlotsPerThread, "HazardPointers::Domain::makeGuard: Thread slot index is out of range");

            return Guard(*this, getHazardIndex(threadIndex, slotInThread));
        }

        void retire(ThreadId threadIndex, void* ptr, Janitor* janitor) noexcept
        {
            assert::debug(threadIndex < MaxThreads, "HazardPointers::Domain::retire: Thread index is out of range");

            auto& retired = _retired[threadIndex];

            if (retired.full())
            {
                scan(threadIndex);

                if (retired.full())
                {
                    assert::debug(false, "HazardPointers::Domain::retire: Hazard pointer retire list full");
                    return;
                }
            }

            retired.push(ptr, janitor);

            // Opportunistic batch scan.
            if ((retired.count & 15u) == 0u)
            {
                scan(threadIndex);
            }
        }

        void scan(ThreadId threadIndex) noexcept
        {
            assert::debug(threadIndex < MaxThreads, "HazardPointers::Domain::scan: Thread index is out of range");

            // Snapshot all hazard pointers into a local fixed array
            std::array<void*, MaxHazards> snapshot {};
            SizeType hazardCount = 0;

            for (SizeType i = 0; i < MaxHazards; ++i)
            {
                void* p = _hazards[i].ptr.load(std::memory_order_acquire);
                if (p != nullptr)
                {
                    snapshot[hazardCount++] = p;
                }
            }

            std::sort(snapshot.begin(), snapshot.begin() + hazardCount);

            auto& retired = _retired[threadIndex];

            SizeType write = 0;
            for (SizeType read = 0; read < retired.count; ++read)
            {
                RetiredNode& retiredNode = retired.nodes[read];

                const bool protectedNow = std::binary_search(snapshot.begin(), snapshot.begin() + hazardCount, retiredNode.ptr);

                if (protectedNow)
                {
                    if (write != read)
                        retired.nodes[write] = retiredNode;
                    ++write;
                }
                else if(retiredNode.janitor != nullptr)
                {
                    retiredNode.janitor->cleanup(retiredNode.ptr);
                }
            }

            retired.count = write;
        }

        void scanAll() noexcept
        {
            for (SizeType i = 0; i < MaxThreads; ++i)
                scan(i);
        }

        bool isHazard(void* p) const noexcept
        {
            for (SizeType i = 0; i < MaxHazards; ++i)
            {
                if (_hazards[i].ptr.load(std::memory_order_acquire) == p)
                    return true;
            }
            return false;
        }

    private:
        static constexpr SizeType getHazardIndex(const ThreadId threadIndex, const SizeType slotInThread) noexcept
        {
            return static_cast<SizeType>(threadIndex) * SlotsPerThread + slotInThread;
        }
    };
}