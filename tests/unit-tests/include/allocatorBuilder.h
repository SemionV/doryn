#pragma once
#include <spdlog/fmt/fmt.h>

#include "dory/memory/allocators/general/segregationAllocator.h"
#include "dory/memory/allocators/general/systemAllocator.h"
#include <dory/memory/allocators/specific/pageBlockAllocator.h>

namespace dory::test_utilities
{
    class AllocatorBuilder
    {
    public:
        constexpr static std::size_t MEMORY_CLASS_COUNT = 11;

        using SystemAllocatorType = memory::allocators::general::SystemAllocator;
        using LargeObjectAllocator = SystemAllocatorType;
        using MemoryBlockNodeAllocatorType = SystemAllocatorType;
        using PageAllocatorType = memory::allocators::specific::PageBlockAllocator;
        using SegregationAllocatorType = memory::allocators::general::SegregationAllocator<MEMORY_CLASS_COUNT, PageAllocatorType, SystemAllocatorType, SystemAllocatorType>;

    private:
        constexpr static std::size_t PAGE_SIZE = 4096;

        PageAllocatorType _blockAllocator;
        LargeObjectAllocator _largeObjectAllocator;
        MemoryBlockNodeAllocatorType _memoryBlockNodeAllocator;

        std::array<memory::allocators::general::MemorySizeClass, MEMORY_CLASS_COUNT> _sizeClasses = {
            memory::allocators::general::MemorySizeClass{ 8, 1024 },
            memory::allocators::general::MemorySizeClass{ 16, 1024 },
            memory::allocators::general::MemorySizeClass{ 32, 1024 },
            memory::allocators::general::MemorySizeClass{ 64, 1024 },
            memory::allocators::general::MemorySizeClass{ 128, 1024 },
            memory::allocators::general::MemorySizeClass{ 256, 1024 },
            memory::allocators::general::MemorySizeClass{ 512, 1024 },
            memory::allocators::general::MemorySizeClass{ 1024, 1024 },
            memory::allocators::general::MemorySizeClass{ 2048, 1024 },
            memory::allocators::general::MemorySizeClass{ 4096, 1024 },
            memory::allocators::general::MemorySizeClass{ 8192, 512 }
        };

    public:
        explicit AllocatorBuilder(memory::profilers::IBlockAllocationProfiler* blockAllocProfiler = nullptr,
            memory::profilers::IAllocatorProfiler* largeObjectAllocProfiler = nullptr,
            memory::profilers::IAllocatorProfiler* memoryBlockNodeAllocProfiler = nullptr);

        std::shared_ptr<SegregationAllocatorType> build(memory::profilers::IAllocatorProfiler* profiler = nullptr);
    };
}
