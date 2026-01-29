#pragma once

#include <refl.hpp>

#include "list.h"
#include <dory/generic/concepts.h>
#include <dory/macros/assert.h>

#include "queue.h"
#include "spinLock.h"

namespace dory::data_structures::containers::lockfree::freelist
{
    template<typename TValue, typename TSlotIndexType>
    struct Slot
    {
        alignas(TValue) std::byte storage[sizeof(TValue)];
        std::atomic<TSlotIndexType> nextSlot = 0;
        std::atomic<TSlotIndexType> generation = 0;
        std::atomic<bool> active = false;

        TValue* data() noexcept
        {
            return std::launder(reinterpret_cast<TValue*>(storage));
        }
        const TValue* data() const noexcept
        {
            return std::launder(reinterpret_cast<const TValue*>(storage));
        }

    };

    template<typename T, typename TAllocator, std::uint32_t SEGMENT_SIZE = 8, std::size_t MAX_SEGMENTS = 1>
    requires(generic::concepts::is_power_of_two<SEGMENT_SIZE> && generic::concepts::is_power_of_two<MAX_SEGMENTS>) // both must be power of two
    class FreeListArray: public SegmentedList<Slot<T, std::uint32_t>, TAllocator, SEGMENT_SIZE, MAX_SEGMENTS>
    {
    public:
        using SlotIndexType = std::uint32_t;
        struct SlotIdentifier
        {
            SlotIndexType index;
            SlotIndexType generation;
        };
        using SlotType = Slot<T, SlotIndexType>;
        using ParentType = SegmentedList<SlotType, TAllocator, SEGMENT_SIZE, MAX_SEGMENTS>;
        using RetiredListType = SegmentedList<SlotIdentifier, TAllocator, SEGMENT_SIZE, MAX_SEGMENTS>;
        using size_type = ParentType::size_type;
        using value_type = T;

        //TODO: fix the assert, mul of two numbers cannot be larger than max int value in any case
        static_assert(SEGMENT_SIZE * MAX_SEGMENTS < std::numeric_limits<SlotIndexType>::max()); //Overflow
    private:
        static constexpr SlotIndexType UNDEFINED_HEAD_INDEX = std::numeric_limits<SlotIndexType>::max();
        std::atomic<SlotIndexType> _size = 0;
        std::atomic<SlotIndexType> _head = UNDEFINED_HEAD_INDEX;
        RetiredListType _retiredSlots;
        SpinLockMutex _mutex;

    public:
        explicit FreeListArray(TAllocator& allocator):
            ParentType(allocator),
            _retiredSlots(allocator)
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
            assert::inhouse(id.index < this->size(), "Invalid identifier index");

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

            //Destruct data object
            slot->data()->~T();

            //Put slot on free list
            SlotIndexType currentHead = _head.load(std::memory_order::relaxed);
            slot->nextSlot.store(currentHead, std::memory_order::relaxed);
            while(!_head.compare_exchange_weak(
                currentHead,
                id.index,
                std::memory_order::release,
                std::memory_order::relaxed))
            {}

            _size.fetch_sub(1, std::memory_order::relaxed);
        }

        /*
         * Very dangerous and direct, but fastest access method, which can return reference to an uninitialized or inactive(removed) item
         */
        T& get(const SlotIdentifier& id)
        {
            assert::inhouse(id.index < this->size(), "Invalid identifier index");

            SlotType* slot = this->getSlot(id.index);
            assert::inhouse(slot, "Cannot get slot, very pity and strange, hm. Someone got some nasty debugging to do;)");

            return *slot->data();
        }

        template<typename F>
        void forEach(F&& f)
        {
            ParentType::forEach([&f](SlotType* slot)
            {
                std::forward<F>(f)(*slot->data());
            });
        }

        /*
         * Store id of a slot in retired list(wait until coalescent state to reclaim the memory of the slot)
         */
        void retire(const SlotIdentifier& id)
        {
            assert::inhouse(id.index < this->size(), "Invalid identifier index");
            _retiredSlots.append(id);
        }

        /*
         * Reclaims the memory of retired slots and cleans up the list of retired slots
         * reclaim should be called in a coalescent state, when no other thread is accessing the slot memory of the list
         */
        void reclaim()
        {
            std::lock_guard lock {_mutex};

            _retiredSlots.forEach([this](SlotIdentifier* id)
            {
                //reclaim slot
                SlotType* slot = this->getSlot(id->index);
                if(slot->generation == id->generation)
                {
                    slot->active.store(false, std::memory_order_relaxed);
                    slot->data()->~T();

                    //Put slot on free list
                    SlotIndexType currentHead = _head.load(std::memory_order::relaxed);
                    slot->nextSlot.store(currentHead, std::memory_order::relaxed);
                    while(!_head.compare_exchange_weak(
                        currentHead,
                        id->index,
                        std::memory_order::release,
                        std::memory_order::relaxed))
                    {}

                    _size.fetch_sub(1, std::memory_order::relaxed);
                }
            });

            _retiredSlots.clear();
        }

    private:
        void initialize()
        {
            const SlotIndexType first = this->append();
            SlotIndexType prev = first;

            for (SlotIndexType n = 1; n < SEGMENT_SIZE; ++n)
            {
                SlotIndexType idx = this->append();
                SlotType* slot = this->getSlot(prev);
                slot->nextSlot.store(idx, std::memory_order_relaxed);
                prev = idx;
            }

            SlotType* slot = this->getSlot(prev);
            slot->nextSlot.store(UNDEFINED_HEAD_INDEX, std::memory_order_relaxed);
            _head.store(first, std::memory_order_release);
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
                const SlotIndexType newHeadIndex = headSlot->nextSlot.load(std::memory_order::relaxed);
                if(_head.compare_exchange_weak(headIndex, newHeadIndex, std::memory_order::acquire, std::memory_order::relaxed))
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

            ::new (static_cast<void*>(slot->storage)) T(std::forward<U>(value));
            slot->active.store(true, std::memory_order::release);

            auto generation = _size.fetch_add(1, std::memory_order::relaxed) + 1;

            return { index, generation };
        }
    };
}