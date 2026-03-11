#pragma once

#include <dory/types.h>

#include "types.h"

namespace dory::data_structures::memory_reclamation
{
    template<typename TDomain>
    class Guard
    {
    private:
        TDomain* _domain = nullptr;
        const ThreadId _threadIndex = {};
        const SizeType _pointerSlot = {};
        bool _enteredCriticalSection = false;

    public:
        Guard() noexcept = default;

        Guard(TDomain& domain, const ThreadId threadIndex, const SizeType pointerSlot):
            _domain(&domain),
            _threadIndex(threadIndex),
            _pointerSlot(pointerSlot)
        {
            _domain->enter(_threadIndex);
            _enteredCriticalSection = true;
        }

        Guard(const Guard&) = delete;
        Guard& operator=(const Guard&) = delete;

        Guard(Guard&& other):
            _domain(other._domain),
            _threadIndex(other._threadIndex),
            _pointerSlot(other._pointerSlot),
            _enteredCriticalSection(other._enteredCriticalSection)
        {
            other._domain = nullptr;
            other._enteredCriticalSection = false;
        }

        Guard& operator=(Guard&&) = delete;

        ~Guard()
        {
            if (_enteredCriticalSection && _domain)
                _domain->leave(_threadIndex);

            reset();
        }

        void reset() noexcept
        {
            if (_domain)
                _domain->clearPointerSlot(_pointerSlot);
        }

        void protectPointer(void* ptr) noexcept
        {
            assert::debug(_domain, "MemoryReclamation::Guard: Domain pointer is nullptr");
            _domain->occupyPointerSlot(ptr, _pointerSlot);
        }

        template <typename T>
        T* protectAtomicPointer(std::atomic<T*>& src) noexcept
        {
            return _domain->occupyAtomicPointerSlot(src, _pointerSlot);
        }
    };
}