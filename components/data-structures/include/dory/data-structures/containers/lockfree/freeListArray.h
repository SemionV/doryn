#pragma once

#include <refl.hpp>

#include "list.h"
#include <dory/generic/concepts.h>
#include <dory/macros/assert.h>

namespace dory::data_structures::containers::lockfree::freelist
{
    template<typename TValue, typename TSlotIndexType>
    struct Slot
    {
        std::byte storage[sizeof(TValue)] {};
        TSlotIndexType nextSlot = 0;
        std::atomic<TSlotIndexType> generation = 0;
        std::atomic<bool> active = false;
    };

    template<typename T, typename TAllocator, std::uint32_t SEGMENT_SIZE = 8, std::size_t MAX_SEGMENTS = 1>
    requires(generic::concepts::is_power_of_two<SEGMENT_SIZE> && generic::concepts::is_power_of_two<MAX_SEGMENTS>) // both must be power of two
    class FreeListArray: public SegmentedList<Slot<T, std::uint32_t>, TAllocator, SEGMENT_SIZE, MAX_SEGMENTS>
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

        static_assert(SEGMENT_SIZE * MAX_SEGMENTS < std::numeric_limits<SlotIndexType>::max()); //Overflow
    private:
        static constexpr SlotIndexType UNDEFINED_HEAD_INDEX = std::numeric_limits<SlotIndexType>::max();
        std::atomic<SlotIndexType> _size = 0;
        std::atomic<SlotIndexType> _head = UNDEFINED_HEAD_INDEX;

    public:
        explicit FreeListArray(TAllocator& allocator):
            ParentType(allocator)
        {
            initialize();
        }

        [[nodiscard]] size_type size() const
        {
            return _size.load(std::memory_order::relaxed);
        }

        SlotIdentifier add(const T& value)
        {
            return addGeneric(value);
        }

        SlotIdentifier add(T&& value)
        {
            return addGeneric(std::forward<T>(value));
        }

        void remove(SlotIdentifier id)
        {
            assert::inhouse(id.index < this->capacity(), "Invalid identifier index");

            SlotType* slot = this->getSlot(id.index);
            assert::inhouse(slot, "Cannot get slot, very pity and strange, hm. Someone got some nasty debugging to do;)");

            //Check if slot was accidentally removed and reallocated prior to this call
            bool isActive = slot->active.load(std::memory_order::acquire);
            SlotIndexType generation = slot->generation.load(std::memory_order::relaxed);
            if(!isActive || generation != id.generation)
            {
                //Slot was allocated or removed by another thread or the same thread prior to this call
                return;
            }

            //Take ownership of the slot
            if(!slot->active.compare_exchange_strong(isActive, false, std::memory_order::release, std::memory_order::relaxed))
            {
                //Slot was removed by another thread
                return;
            }

            //Put slot on free list
            SlotIndexType currentHead = _head.load(std::memory_order::relaxed);
            while(!_head.compare_exchange_weak(
                currentHead,
                id.index,
                std::memory_order::release,
                std::memory_order::relaxed));

            _size.fetch_sub(1, std::memory_order::relaxed);
        }

    private:
        void initialize()
        {
            this->reserve(SEGMENT_SIZE);

            const SlotIndexType headIndex = this->append();
            SlotType* headSlot = this->getSlot(headIndex);
            headSlot->nextSlot = UNDEFINED_HEAD_INDEX;
            SlotIndexType prevIndex = headIndex;
            SlotIndexType i = prevIndex;

            while(i < SEGMENT_SIZE - 1)
            {
                const size_type slotIndex = this->append();
                SlotType* slot = this->getSlot(prevIndex);
                slot->nextSlot = UNDEFINED_HEAD_INDEX;

                SlotType* prevSlot = this->getSlot(prevIndex);
                prevSlot->nextSlot = slotIndex;
                prevIndex = slotIndex;

                ++i;
            }

            _head.store(headIndex, std::memory_order::release);
        }

        template<typename U>
        SlotIdentifier addGeneric(U&& value)
        {
            SlotType* slot = nullptr;
            SlotIndexType index = UNDEFINED_HEAD_INDEX;

            SlotIndexType headIndex = _head.load(std::memory_order::relaxed);
            while(headIndex != UNDEFINED_HEAD_INDEX)
            {
                SlotType* headSlot = this->getSlot(headIndex);
                const SlotIndexType newHeadIndex = headSlot->nextSlot;
                if(_head.compare_exchange_weak(headIndex, newHeadIndex, std::memory_order::release, std::memory_order::relaxed))
                {
                    slot = headSlot;
                    slot->generation.fetch_add(1, std::memory_order::relaxed);
                    index = headIndex;
                    break;
                }
            }

            if(slot == nullptr)
            {
                index = this->append();
                slot = this->getSlot(index);
            }

            assert::inhouse(slot, "Cannot allocate slot");

            ::new (static_cast<void*>(slot->storage)) U(std::forward<U>(value));
            slot->active.store(true, std::memory_order::release);

            _size.fetch_add(1, std::memory_order::relaxed);

            return { index, slot->generation };
        }
    };
}