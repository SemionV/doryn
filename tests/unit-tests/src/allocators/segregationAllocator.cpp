#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dory/memory/allocators/general/segregationAllocator.h>
#include <allocatorProfiler.h>

#include "dory/memory/allocators/general/systemAllocator.h"
#include "dory/memory/profilers/blockAuditProfiler.h"
#include <dory/memory/allocators/pageAllocator.h>

using namespace dory::test_utilities;

TEST(SegregationAllocatorTests, allocateObject)
{
    Profiler systemAllocProfiler;
    Profiler segregationAllocProfiler;

    constexpr static std::size_t PAGE_SIZE = 4096;
    constexpr static std::size_t MEMORY_CLASS_COUNT = 11;

    dory::memory::profilers::BlockAuditProfiler _blockAllocProfiler;
    dory::memory::PageAllocator _blockAllocator { PAGE_SIZE, &_blockAllocProfiler };
    dory::memory::allocators::general::SystemAllocator _systemAllocator { &systemAllocProfiler };
    std::array _sizeClasses = {
        dory::memory::allocators::general::MemorySizeClass{ 8, 1024 },
        dory::memory::allocators::general::MemorySizeClass{ 16, 1024 },
        dory::memory::allocators::general::MemorySizeClass{ 32, 1024 },
        dory::memory::allocators::general::MemorySizeClass{ 64, 1024 },
        dory::memory::allocators::general::MemorySizeClass{ 128, 1024 },
        dory::memory::allocators::general::MemorySizeClass{ 256, 1024 },
        dory::memory::allocators::general::MemorySizeClass{ 512, 1024 },
        dory::memory::allocators::general::MemorySizeClass{ 1024, 1024 },
        dory::memory::allocators::general::MemorySizeClass{ 2048, 1024 },
        dory::memory::allocators::general::MemorySizeClass{ 4096, 1024 },
        dory::memory::allocators::general::MemorySizeClass{ 8192, 512 }
    };

    using SegregationAllocatorType = dory::memory::allocators::general::SegregationAllocator<MEMORY_CLASS_COUNT,
        dory::memory::PageAllocator,
        dory::memory::allocators::general::SystemAllocator,
        dory::memory::allocators::general::SystemAllocator>;

    SegregationAllocatorType allocator { _blockAllocator, _systemAllocator, _systemAllocator, &segregationAllocProfiler, _sizeClasses };

    {
        int* obj = allocator.allocateObject<int>({});

        EXPECT_TRUE(obj);

        allocator.deallocateObject(obj);
    }

    EXPECT_EQ(segregationAllocProfiler.objectCounters.sizeAllocated, sizeof(int));
    EXPECT_EQ(segregationAllocProfiler.objectCounters.sizeFreed, sizeof(int));
    EXPECT_EQ(segregationAllocProfiler.objectCounters.allocatedCount, 1);
    EXPECT_EQ(segregationAllocProfiler.objectCounters.freedCount, 1);
}