#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dory/memory/allocators/general-purpose/systemAllocator.h>

TEST(SystemAllocatorTests, allocateObject)
{
    dory::memory::allocators::general_purpose::SystemAllocator allocator { nullptr };

    int* obj = allocator.allocateObject<int>({});

    EXPECT_TRUE(obj);
}

struct TestType
{
    int mem1 = 0;
    int& destructed;

    TestType(int& destructed):
        destructed(destructed)
    {
        mem1 = 1;
    }

    ~TestType()
    {
        ++destructed;
    }
};

TEST(SystemAllocatorTests, allocateAndConstructObject)
{
    dory::memory::allocators::general_purpose::SystemAllocator allocator { nullptr };

    int destructed = 0;
    {
        const auto obj = allocator.allocateObject<TestType>({}, destructed);

        EXPECT_TRUE(obj);
        EXPECT_EQ(obj->mem1, 1);

        allocator.deallocateObject(obj);
    }

    EXPECT_EQ(destructed, 1);
}

TEST(SystemAllocatorTests, allocateAndConstructArray)
{
    int constexpr arraySize = 8;

    dory::memory::allocators::general_purpose::SystemAllocator allocator { nullptr };

    int destructed = 0;
    {
        const auto obj = allocator.allocateArray<TestType>({}, arraySize, destructed);

        EXPECT_TRUE(obj);

        for(int i = 0; i < arraySize; ++i)
        {
            const TestType* entry = obj + 1;
            EXPECT_EQ(entry->mem1, 1);

        }

        allocator.deallocateArray(obj, arraySize);
    }

    EXPECT_EQ(destructed, arraySize);
}