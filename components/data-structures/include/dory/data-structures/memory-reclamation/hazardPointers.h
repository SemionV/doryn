#pragma once

#include <atomic>
#include <array>
#include <algorithm>

#include <dory/types.h>
#include <dory/constants.h>
#include <dory/macros/assert.h>

#include "janitor.h"
#include "retireList.h"
#include "guard.h"
#include "domain.h"

namespace dory::data_structures::memory_reclamation::hazard_pointers
{
    struct alignas(constants::CacheLineSize) HazardSlot
    {
        std::atomic<void*> ptr { nullptr };
    };

    template <typename TDomainTraits>
    class HazardPointersDomain;

    template<SizeType MaxThreads, SizeType PointerSlotsPerThread, SizeType MaxRetiredPerThread>
    struct HazardPointersDomainTraits
    {
        using RetiredNodeType = RetiredNode;
        using RetireListType = RetireList<RetiredNodeType, MaxRetiredPerThread>;
        using GuardType = Guard<HazardPointersDomain<HazardPointersDomainTraits>>;
        static constexpr SizeType maxThreads = MaxThreads;
        static constexpr SizeType maxRetiredPerThread = MaxRetiredPerThread;
        static constexpr SizeType pointerSlotsPerThread = PointerSlotsPerThread;
        static constexpr SizeType maxHazards = MaxThreads * PointerSlotsPerThread;
    };

    template <typename TDomainTraits>
    class HazardPointersDomain: public MemoryReclamationDomain<HazardPointersDomain<TDomainTraits>, TDomainTraits>
    {
        friend class MemoryReclamationDomain<HazardPointersDomain, TDomainTraits>;

    private:
        std::array<HazardSlot, TDomainTraits::maxHazards> _hazards {};

    private:
        static SizeType getPointerSlotImpl(ThreadId threadId, PointerToken pointerToken)
        {
            return getHazardIndex(threadId, pointerToken);
        }

        static void enterImpl(ThreadId threadId) noexcept
        {}

        static void leaveImpl(ThreadId threadId) noexcept
        {}

        void clearPointerSlotImpl(const SizeType pointerSlot)
        {
            _hazards[pointerSlot].ptr.store(nullptr, std::memory_order_release);
        }

        void occupyPointerSlotImpl(void* ptr, const SizeType pointerSlot)
        {
            _hazards[pointerSlot].ptr.store(ptr, std::memory_order_release);
        }

        template <typename T>
        T* occupyAtomicPointerSlotImpl(std::atomic<T*>& src, const SizeType pointerSlot)
        {
            T* ptr = nullptr;
            do
            {
                ptr = src.load(std::memory_order_acquire);
                _hazards[pointerSlot].ptr.store(ptr, std::memory_order_release);
            }
            while (ptr != src.load(std::memory_order_acquire));

            return ptr;
        }

        void retireImpl(ThreadId threadIndex, void* ptr, Janitor* janitor) noexcept
        {
            assert::debug(threadIndex < TDomainTraits::maxThreads, "HazardPointers::Domain::retire: Thread index is out of range");

            auto& retired = this->_retired[threadIndex];

            if (retired.full())
            {
                this->collect(threadIndex);

                if (retired.full())
                {
                    assert::debug(false, "HazardPointers::Domain::retire: Hazard pointer retire list full");
                    return;
                }
            }

            retired.push(typename TDomainTraits::RetiredNodeType { ptr, janitor });

            // Opportunistic batch scan.
            if ((retired.count & 15u) == 0u)
            {
                this->collect(threadIndex);
            }
        }

        void collectImpl(ThreadId threadId) noexcept
        {
            assert::debug(threadId < TDomainTraits::maxThreads, "HazardPointers::Domain::scan: Thread index is out of range");

            // Snapshot all hazard pointers into a local fixed array
            std::array<void*, TDomainTraits::maxHazards> snapshot {};
            SizeType hazardCount = 0;

            for (SizeType i = 0; i < TDomainTraits::maxHazards; ++i)
            {
                void* p = _hazards[i].ptr.load(std::memory_order_acquire);
                if (p != nullptr)
                {
                    snapshot[hazardCount++] = p;
                }
            }

            std::sort(snapshot.begin(), snapshot.begin() + hazardCount);

            auto& retiredList = this->_retired[threadId];

            retiredList.reclaimIf([&snapshot, hazardCount](const typename TDomainTraits::RetiredNodeType& node)
            {
                return !std::binary_search(snapshot.begin(), snapshot.begin() + hazardCount, node.ptr);
            });
        }

        static void tryAdvanceEpochImpl()
        {}

        static constexpr SizeType getHazardIndex(const ThreadId threadIndex, const SizeType slotInThread) noexcept
        {
            return static_cast<SizeType>(threadIndex) * TDomainTraits::pointerSlotsPerThread + slotInThread;
        }

    public:
        bool isHazard(void* p) const noexcept
        {
            for (SizeType i = 0; i < TDomainTraits::maxHazards; ++i)
            {
                if (_hazards[i].ptr.load(std::memory_order_acquire) == p)
                    return true;
            }
            return false;
        }
    };
}