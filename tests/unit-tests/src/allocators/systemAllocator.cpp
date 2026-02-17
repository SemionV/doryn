#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dory/memory/allocators/general-purpose/systemAllocator.h>

//TODO: check if constructor is called
//TODO: check if destructor is called

TEST(SystemAllocatorTests, simpleAllocation)
{
    dory::memory::allocators::general_purpose::SystemAllocator allocator { nullptr };

    int* obj = allocator.allocateObject<int>({});

    EXPECT_TRUE(obj);
}