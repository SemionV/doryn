#pragma once

#include <atomic>
#include <dory/memory/resources/memoryBlock.h>
#include <dory/memory/resources/resourceHandle.h>

namespace dory::memory
{
    class DynamicAllocator
    {
    private:
        MemoryBlock _memory;

        struct AddressHandle
        {
            std::size_t address {}; //TODO: possible to reduce size to 32 bits
            std::atomic<bool> locked {};
        };
    public:
        template <typename T> friend class ResourceHandle<T>;

        explicit DynamicAllocator(const MemoryBlock& memoryBlock) noexcept;

        template<typename T>
        ResourceHandle<T> allocate(std::size_t size) noexcept
        {
        }

        template<typename T>
        void deallocate(ResourceHandle<T>& handle) noexcept
        {
        }
    };
}