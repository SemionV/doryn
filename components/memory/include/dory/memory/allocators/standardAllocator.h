#pragma once

namespace dory::memory
{
    template<typename T, typename TGenericAllocator>
    //TODO: delete this class and use std::pmr::polymorphic_allocator with std::pmr::memory_resource instead
    //TODO: this allocator should be used in standard containers like std::vector, std::shared_ptr, etc, rename it accordingly or document this about the class
    class StandardAllocator
    {
    public:
        using value_type = T;

        TGenericAllocator& _genericAllocator;

        explicit StandardAllocator(TGenericAllocator& genericAllocator):
            _genericAllocator(genericAllocator)
        {}

        template<typename U>
        explicit StandardAllocator(const StandardAllocator<U, TGenericAllocator>& other) noexcept
        : _genericAllocator(other._genericAllocator) {}

        //TODO: alignment of T should be taken in consideration, call allocateBytes for n object of type T, do not construct them
        //TODO: handle overflow if size of allocated chunk of memory is larger than std::size_t
        T* allocate(const std::size_t n)
        {
            return static_cast<T*>(_genericAllocator.allocate(n * sizeof(T)));
        }

        void deallocate(T* ptr, std::size_t n)
        {
            _genericAllocator.deallocate(ptr);
        }

        // equality
        template<class U>
        bool operator==(const StandardAllocator<U, TGenericAllocator>& other) const noexcept
        {
            return _genericAllocator == other._genericAllocator;
        }
        template<class U>
        bool operator!=(const StandardAllocator<U, TGenericAllocator>& other) const noexcept
        {
            return !(*this == other);
        }
    };
}