#pragma once

namespace dory::memory
{
    class SystemAllocator
    {
    public:
        void* allocate(const std::size_t size)
        {
            return ::operator new(size);
        }

        void deallocate(void* ptr)
        {
            ::operator delete(ptr);
        }

        bool isInRange(void* ptr) const
        {
            return true;
        }
    };
}