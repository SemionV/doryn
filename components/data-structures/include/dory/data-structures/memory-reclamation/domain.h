#pragma once

#include <dory/base.h>
#include "types.h"

namespace dory::data_structures::memory_reclamation
{
    template <typename TImplementation, typename TDomainTraits>
    class MemoryReclamationDomain: public Base<TImplementation>
    {
    private:
        std::atomic_flag _collecting = ATOMIC_FLAG_INIT;

    protected:
        std::array<typename TDomainTraits::RetireListType, TDomainTraits::maxThreads> _retired {};

    public:
        MemoryReclamationDomain() = default;

        ~MemoryReclamationDomain()
        {
            drain();
        }

        //Map pointer token to a pointer slot in domain
        SizeType getPointerSlot(ThreadId threadId, PointerToken pointerToken)
        {
            return this->implRef().getPointerSlotImpl(threadId, pointerToken);
        }

        //Enter critical section
        void enter(ThreadId threadId) noexcept
        {
            this->implRef().enterImpl(threadId);
        }

        //Leave critical section
        void leave(ThreadId threadId) noexcept
        {
            this->implRef().leaveImpl(threadId);
        }

        //Release pointer from slot
        void clearPointerSlot(const SizeType pointerSlot)
        {
            this->implRef().clearPointerSlotImpl(pointerSlot);
        }

        //Occupy slot with pointer
        void occupyPointerSlot(void* ptr, const SizeType pointerSlot)
        {
            this->implRef().occupyPointerSlotImpl(ptr, pointerSlot);
        }

        //Occupy slot with atomic pointer
        template <typename T>
        T* occupyAtomicPointerSlot(std::atomic<T*>& src, const SizeType pointerSlot)
        {
            return this->implRef().occupyAtomicPointerSlotImpl(src, pointerSlot);
        }

        //Try to advance epoch(frame)
        void tryAdvanceEpoch()
        {
            this->implRef().tryAdvanceEpochImpl();
        }

        //TODO: generalize retire implementation
        void retire(ThreadId threadId, void* ptr, Janitor* janitor) noexcept
        {
            this->implRef().retireImpl(threadId, ptr, janitor);
        }

        //TODO: generalize collect implementation
        void collect(ThreadId threadId) noexcept
        {
            this->implRef().collectImpl(threadId);
        }

        TDomainTraits::GuardType makeGuard(const ThreadId threadIndex, const PointerToken pointerToken = 0)
        {
            assert::debug(threadIndex < TDomainTraits::maxThreads, "MemoryReclamation::Domain::makeGuard: Thread index is out of range");
            assert::debug(pointerToken < TDomainTraits::pointerSlotsPerThread, "MemoryReclamation::Domain::makeGuard: Thread slot index is out of range");

            return typename TDomainTraits::GuardType(this->implRef(), threadIndex, getPointerSlot(threadIndex, pointerToken));
        }

        static constexpr SizeType getMaxThreads()
        {
            return TDomainTraits::maxThreads;
        }

        static constexpr SizeType getSlotsPerThread()
        {
            return TDomainTraits::pointerSlotsPerThread;
        }

        void drain()
        {
            collectAll();
        }

        void collectAll() noexcept
        {
            //Check if some thread is doing collection currently
            if(_collecting.test_and_set(std::memory_order_acquire))
                return;

            tryAdvanceEpoch();

            for (SizeType i = 0; i < TDomainTraits::maxThreads; ++i)
                collect(i);

            _collecting.clear(std::memory_order_release);
        }
    };
}