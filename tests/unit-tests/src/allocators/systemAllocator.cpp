#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dory/memory/allocators/general-purpose/systemAllocator.h>

TEST(SystemAllocatorTests, simpleAllocation)
{
    dory::memory::allocators::general_purpose::SystemAllocator allocator { nullptr };

    int* obj = allocator.allocateObject<int>({});

    EXPECT_TRUE(obj);
}