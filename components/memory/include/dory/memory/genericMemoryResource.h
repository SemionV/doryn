#pragma once

#include <memory_resource>
#include <dory/types.h>

namespace dory::memory
{
    template<typename TAllocator>
    class GenericMemoryResource final : public std::pmr::memory_resource
    {
    private:
        LabelType _label;

    public:
        explicit GenericMemoryResource(TAllocator& allocator, LabelType label = {}) noexcept:
            _allocator(&allocator),
        _label(label)
        {}

    private:
        TAllocator* _allocator;

        void* do_allocate(const std::size_t bytes, std::size_t alignment) override
        {
            return _allocator->allocateBytes(LabelType{}, bytes, alignment);
        }

        void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
        {
            _allocator->deallocateBytes(p, bytes, alignment);
        }

        [[nodiscard]] bool do_is_equal(const memory_resource& other) const noexcept override
        {
            return this == &other;
        }
    };
}