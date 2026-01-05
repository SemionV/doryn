#pragma once

#include "list.h"
#include <dory/generic/concepts.h>
#include <dory/macros/assert.h>

namespace dory::data_structures::containers::lockfree::freelist
{
    template<typename TValue, typename TSlotIndexType>
    requires(sizeof(TSlotIndexType) <= sizeof(TValue))
    struct Slot
    {
        union
        {
            TValue value;
            TSlotIndexType nextSlot = 0;
        };
        std::atomic<TSlotIndexType> generation = 0;
        std::atomic<bool> active = false;
    };

    template<typename T, typename TAllocator, std::size_t SEGMENT_SIZE = 8, std::size_t MAX_SEGMENTS = 1>
    requires(generic::concepts::is_power_of_two<SEGMENT_SIZE> && generic::concepts::is_power_of_two<MAX_SEGMENTS>) // both must be power of two
    class FreeList: SegmentedList<Slot<T, std::uint32_t>, TAllocator, SEGMENT_SIZE, MAX_SEGMENTS>
    {
    public:
        using SlotIndexType = std::uint32_t;
        using SlotType = Slot<T, SlotIndexType>;
        using ParentType = SegmentedList<SlotType, TAllocator, SEGMENT_SIZE, MAX_SEGMENTS>;
        using size_type = ParentType::size_type;

        struct SlotIdentifier
        {
            SlotIndexType index;
            SlotIndexType generation;
        };
    private:
        static constexpr size_type UNDEFINED_HEAD_INDEX = std::numeric_limits<SlotIndexType>::max();
        std::atomic<SlotIndexType> _head = UNDEFINED_HEAD_INDEX;

    public:
        template<typename... TSlotArgs>
        explicit FreeList(TAllocator& allocator, TSlotArgs&&... slotArgs):
            ParentType(allocator)
        {}

    private:
        template<typename... TSlotArgs>
        void initialize(TSlotArgs&&... slotArgs)
        {
            this->reserve(SEGMENT_SIZE);

            for(size_type i = 0; i < SEGMENT_SIZE; ++i)
            {
                const size_type slotIndex = this->append();
                SlotType* slot = this->getSlot(slotIndex);
                assert::debug(slot, "Cannot get slot pointer");

                SlotIndexType currentHead = _head.load(std::memory_order::acquire);

                slot->nextSlot = currentHead;
                while(!_head.compare_exchange_weak(currentHead, slotIndex, std::memory_order::release, std::memory_order::acquire))
                {
                    slot->nextSlot = currentHead;
                }

                slot->active.store(true, std::memory_order::release);
            }
        }
    };
}