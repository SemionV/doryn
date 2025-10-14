#pragma once

namespace dory::memory
{
    template <class T>
    class KernelDynamicAllocator
    {
    public:
        using value_type = T;

        KernelDynamicAllocator() noexcept = default;

        template <class U>
        explicit KernelDynamicAllocator(const KernelDynamicAllocator<U>&) noexcept {}

        T* allocate()
        {
            return static_cast<T*>(::operator new(sizeof(T)));
        }

        void deallocate(T* p) noexcept
        {
            ::operator delete(p);
        }
    };
}