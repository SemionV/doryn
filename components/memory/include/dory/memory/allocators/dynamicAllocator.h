#pragma once

#include <atomic>
#include <dory/memory/resources/memoryBlock.h>
#include <dory/memory/resources/resourceHandle.h>
#include "poolAllocator.h"

namespace dory::memory
{
    struct BlockDescriptor
    {
        std::size_t offset;
        std::size_t size;
        std::size_t 
    };

    struct AddressHandle
    {
        std::size_t offset {}; //TODO: possible to reduce size to 32 bits
        std::atomic<bool> locked {};
    };

    struct AddressList
    {
        AddressHandle* handles;
        std::size_t handlesCount;
    };

    class DynamicAllocator
    {
    private:
        MemoryBlock _memory;
        AddressList _addressHandles;

    public:
        template <typename T> friend class ResourceHandle<T>;

        explicit DynamicAllocator(const MemoryBlock& memoryBlock, AddressList addressHandles) noexcept;

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