#pragma once

#include <atomic>
#include <array>
#include <algorithm>

#include <dory/types.h>
#include <dory/constants.h>
#include <dory/macros/assert.h>

#include "janitor.h"
#include "retireList.h"

namespace dory::data_structures::memory_reclamation::hazard_pointers
{
    using SizeType = u32;

    struct alignas(constants::CacheLineSize) HazardSlot
    {
        std::atomic<void*> ptr { nullptr };
    };

    template <SizeType MaxThreads, SizeType SlotsPerThread, SizeType MaxRetiredPerThread>
    class Domain
    {
    public:
        using RetiredNodeType = RetiredNode;
        using RetireListType = RetireList<RetiredNodeType, MaxRetiredPerThread>;

        static constexpr SizeType MaxHazards = MaxThreads * SlotsPerThread;

    private:
        std::array<HazardSlot, MaxHazards> _hazards {};
        std::array<RetireListType, MaxThreads> _retired {};

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

        ~Domain()
        {
            drain();
        }

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
                collect(threadIndex);

                if (retired.full())
                {
                    assert::debug(false, "HazardPointers::Domain::retire: Hazard pointer retire list full");
                    return;
                }
            }

            retired.push(RetiredNodeType { ptr, janitor });

            // Opportunistic batch scan.
            if ((retired.count & 15u) == 0u)
            {
                collect(threadIndex);
            }
        }

        void collect(ThreadId threadIndex) noexcept
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

            auto& retiredList = _retired[threadIndex];

            retiredList.reclaimIf([&snapshot, hazardCount](const RetiredNodeType& node)
            {
                return !std::binary_search(snapshot.begin(), snapshot.begin() + hazardCount, node.ptr);
            });
        }

        void collectAll() noexcept
        {
            for (SizeType i = 0; i < MaxThreads; ++i)
                collect(i);
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

        void drain()
        {
            collectAll();
        }

    private:
        static constexpr SizeType getHazardIndex(const ThreadId threadIndex, const SizeType slotInThread) noexcept
        {
            return static_cast<SizeType>(threadIndex) * SlotsPerThread + slotInThread;
        }
    };
}