#pragma once

#include <cstddef>
#include <cstdint>

#include "../resources/memoryBlock.h"

namespace dory::memory
{
    class BumpAllocator
    {
    private:
        MemoryBlock& _memoryBlock;
        std::uintptr_t _free {};
        std::size_t _size {};

    public:
        explicit BumpAllocator(MemoryBlock& memoryBlock);

        void* allocate(std::size_t size, std::uint8_t align = 0) noexcept;
        static void deallocate(void* ptr);
        [[nodiscard]] std::size_t getAllocatedSize() const;
    };
}