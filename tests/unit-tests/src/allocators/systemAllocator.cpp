#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dory/memory/allocators/general/systemAllocator.h>
#include <allocatorProfiler.h>

using namespace dory::test_utilities;

TEST(SystemAllocatorTests, allocateObject)
{
    Profiler profiler;
    dory::memory::allocators::general::SystemAllocator allocator { &profiler };

    {
        int* obj = allocator.allocateObject<int>({});

        EXPECT_TRUE(obj);

        allocator.deallocateObject(obj);
    }

    EXPECT_EQ(profiler.objectCounters.sizeAllocated, sizeof(int));
    EXPECT_EQ(profiler.objectCounters.sizeFreed, sizeof(int));
    EXPECT_EQ(profiler.objectCounters.allocatedCount, 1);
    EXPECT_EQ(profiler.objectCounters.freedCount, 1);
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
    Profiler profiler;
    dory::memory::allocators::general::SystemAllocator allocator { &profiler };

    int destructed = 0;
    {
        const auto obj = allocator.allocateObject<TestType>({}, destructed);

        EXPECT_TRUE(obj);
        EXPECT_EQ(obj->mem1, 1);

        allocator.deallocateObject(obj);
    }

    EXPECT_EQ(destructed, 1);

    EXPECT_EQ(profiler.objectCounters.sizeAllocated, sizeof(TestType));
    EXPECT_EQ(profiler.objectCounters.sizeFreed, sizeof(TestType));
    EXPECT_EQ(profiler.objectCounters.allocatedCount, 1);
    EXPECT_EQ(profiler.objectCounters.freedCount, 1);
}

TEST(SystemAllocatorTests, allocateAndConstructArray)
{
    std::size_t constexpr arraySize = 8;

    Profiler profiler;
    dory::memory::allocators::general::SystemAllocator allocator { &profiler };

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

    EXPECT_EQ(profiler.arrayCounters.sizeAllocated, sizeof(TestType) * arraySize);
    EXPECT_EQ(profiler.arrayCounters.sizeFreed, sizeof(TestType) * arraySize);
    EXPECT_EQ(profiler.arrayCounters.allocatedCount, 1);
    EXPECT_EQ(profiler.arrayCounters.freedCount, 1);
}

TEST(SystemAllocatorTests, allocateBytes)
{
    std::size_t constexpr blockSize = 4096;

    Profiler profiler;
    dory::memory::allocators::general::SystemAllocator allocator { &profiler };

    void* ptr = allocator.allocateBytes({}, blockSize);
    EXPECT_TRUE(ptr);

    allocator.deallocateBytes(ptr, blockSize);

    EXPECT_EQ(profiler.bytesCounters.sizeAllocated, blockSize);
    EXPECT_EQ(profiler.bytesCounters.sizeFreed, blockSize);
    EXPECT_EQ(profiler.bytesCounters.allocatedCount, 1);
    EXPECT_EQ(profiler.bytesCounters.freedCount, 1);
}