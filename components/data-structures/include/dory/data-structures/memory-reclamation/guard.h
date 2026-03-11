#pragma once

#include <dory/types.h>

namespace dory::data_structures::memory_reclamation
{
    using PointerToken = u32;

    template<typename TDomain>
    class Guard
    {
    private:
        TDomain* _domain = nullptr;
        const ThreadId _threadIndex = {};
        const PointerToken _pointerToken = {};
        bool _enteredCriticalSection = false;

    public:
        Guard() noexcept = default;

        Guard(TDomain& domain, const ThreadId threadIndex, const PointerToken pointerToken):
            _domain(&domain),
            _threadIndex(threadIndex),
            _pointerToken(pointerToken)
        {
            _domain->enter(_threadIndex);
        }

        Guard(const Guard&) = delete;
        Guard& operator=(const Guard&) = delete;

        Guard(Guard&& other):
            _domain(other._domain),
            _threadIndex(other._threadIndex),
            _pointerToken(other._pointerToken),
            _enteredCriticalSection(other._enteredCriticalSection)
        {
            other._domain = nullptr;
            other._enteredCriticalSection = false;
        }

        Guard& operator=(Guard&&) = delete;

        ~Guard()
        {
            reset();

            if (_enteredCriticalSection && _domain)
                _domain->leave(_threadIndex);
        }

        void reset() noexcept
        {
            if (_domain)
            {
                _domain->clearPointerSlot(_pointerToken);
                _domain = nullptr;
            }
        }

        void protectPointer(void* ptr) noexcept
        {
            assert::debug(_domain, "MemoryReclamation::Guard: Domain pointer is nullptr");
            _domain->occupyPointerSlot(ptr, _pointerToken);
        }

        template <typename T>
        T* protectAtomicPointer(std::atomic<T*>& src) noexcept
        {
            return _domain->occupyAtomicPointerSlot(src, _pointerToken);
        }
    };
}