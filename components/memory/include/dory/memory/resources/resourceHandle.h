#pragma once
#include <cstddef>

namespace dory::memory
{
    template<typename TResource, typename TAllocator>
    class ResourcePointer
    {
    private:
        TResource* _pointer {};
        std::size_t _id {};
        TAllocator& _allocator;

    public:
        explicit ResourcePointer(TResource* pointer, const std::size_t id, TAllocator& allocator) noexcept:
            _pointer(pointer), _id(id), _allocator(allocator)
        {}

        ~ResourcePointer()
        {
            _allocator.unlock(_id);
            _pointer = nullptr;
        }

        explicit operator bool() const
        {
            return _pointer;
        }

        TResource& operator*()
        {
            return *_pointer;
        }

        TResource& operator->()
        {
            return *_pointer;
        }
    };

    template<typename TResource>
    class ResourceHandle
    {
    private:
        std::size_t _id {};

    public:
        explicit ResourceHandle(std::size_t id) noexcept:
            _id(id)
        {}

        template<typename TAllocator>
        ResourcePointer<TResource, TAllocator> lock(TAllocator& allocator)
        {
            return allocator.lock(_id);
        }
    };
}
