#pragma once

namespace dory::memory
{
    template<typename T, typename TGenericAllocator>
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