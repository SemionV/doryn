#pragma once

namespace dory::generic::memory
{
    template<typename TResource, typename TAllocator>
    class AllocationResource
    {
    private:
        TAllocator& _allocator;
        TResource* _resourcePtr = nullptr;

    public:
        template<typename... TArgs>
        explicit AllocationResource(TAllocator& allocator, TArgs&&... args):
            _allocator(allocator)
        {
            _resourcePtr = _allocator.template allocate<TResource>(std::forward<TArgs>()...);
        }

        ~AllocationResource()
        {
            reset();
        }

        AllocationResource(const AllocationResource&) = delete;
        AllocationResource& operator=(const AllocationResource&) = delete;

        AllocationResource(AllocationResource&& other) noexcept
        : _allocator(other._allocator), _resourcePtr(other._resourcePtr)
        {
            other._resourcePtr = nullptr;
        }

        AllocationResource& operator=(AllocationResource&& other) noexcept
        {
            if (this != &other)
            {
                reset();

                _allocator = other._allocator;
                _resourcePtr = other._resourcePtr;

                other._resourcePtr = nullptr;
            }
            return *this;
        }

        [[nodiscard]] TResource* release() noexcept
        {
            TResource* tmp = _resourcePtr;
            _resourcePtr = nullptr;
            return tmp;
        }

        void reset(TResource* newPtr = nullptr)
        {
            if (_resourcePtr)
            {
                _allocator.template deallocateType<TResource>(_resourcePtr);
            }
            _resourcePtr = newPtr;
        }

        [[nodiscard]] TResource* get() const noexcept { return _resourcePtr; }
        [[nodiscard]] TResource& operator*() const noexcept { return *_resourcePtr; }
        [[nodiscard]] TResource* operator->() const noexcept { return _resourcePtr; }

        explicit operator bool() const noexcept { return _resourcePtr != nullptr; }
    };
}
