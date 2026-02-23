#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "dory/memory/allocators/specific/pageBlockAllocator.h"
#include <blockAllocationProfiler.h>

TEST(PageBlockAllocatorTests, allocatePage)
{
    constexpr std::size_t PAGE_SIZE = 4096;
    constexpr dory::LabelType LABEL = 1;
    constexpr dory::LabelType MULTIPLE = 1;

    auto profiler = dory::test_utilities::BlockAllocationProfiler{};
    auto allocator = dory::memory::allocators::specific::PageBlockAllocator{ PAGE_SIZE, &profiler };

    EXPECT_EQ(allocator.getBlockSize(), PAGE_SIZE);

    void* ptr = allocator.allocateBlock(LABEL, MULTIPLE);
    EXPECT_TRUE(ptr != nullptr);

    EXPECT_EQ(profiler.blockAllocations.size(), 1);
    const auto& blockAllocation = profiler.blockAllocations[0];
    EXPECT_EQ(blockAllocation.ptr, ptr);
    EXPECT_EQ(blockAllocation.size, PAGE_SIZE * MULTIPLE);
    EXPECT_EQ(blockAllocation.alignment, PAGE_SIZE);
    EXPECT_EQ(blockAllocation.label, LABEL);

    allocator.deallocateBlock(ptr, MULTIPLE);

    EXPECT_EQ(profiler.blockFrees.size(), 1);
    const auto& blockFree = profiler.blockFrees[0];
    EXPECT_EQ(blockFree.ptr, ptr);
    EXPECT_EQ(blockFree.size, PAGE_SIZE * MULTIPLE);
    EXPECT_EQ(blockFree.alignment, PAGE_SIZE);
}