#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dory/memory/allocators/general-purpose/systemAllocator.h>

//TODO: check if destructor is called

TEST(SystemAllocatorTests, allocateObject)
{
    dory::memory::allocators::general_purpose::SystemAllocator allocator { nullptr };

    int* obj = allocator.allocateObject<int>({});

    EXPECT_TRUE(obj);
}

struct TestType
{
    int mem1 = 0;
    bool& destructed;

    TestType(bool& destructed):
        destructed(destructed)
    {
        mem1 = 1;
    }

    ~TestType()
    {
        destructed = true;
    }
};

TEST(SystemAllocatorTests, allocateAndConstructObject)
{
    dory::memory::allocators::general_purpose::SystemAllocator allocator { nullptr };

    bool destructed = false;
    {
        const auto obj = allocator.allocateObject<TestType>({}, destructed);

        EXPECT_TRUE(obj);
        EXPECT_EQ(obj->mem1, 1);

        allocator.deallocateObject(obj);
    }

    EXPECT_TRUE(destructed);
}