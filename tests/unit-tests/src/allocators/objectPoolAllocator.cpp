#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <allocatorProfiler.h>
#include <dory/memory/allocators/specific/objectPoolAllocator.h>
#include <dory/memory/allocators/general/systemAllocator.h>
#include <dory/memory/allocators/pageAllocator.h>

using namespace dory::test_utilities;

TEST(ObjectPoolAllocatorTests, allocateObject)
{
    constexpr std::size_t PAGE_SIZE = 4096;

    using PageAllocatorType = dory::memory::PageAllocator;
    using SystemAllocatorType = dory::memory::allocators::general::SystemAllocator;

    PageAllocatorType pageAllocator { PAGE_SIZE, nullptr };
    SystemAllocatorType systemAllocator { nullptr };
    dory::memory::allocators::specific::ObjectPoolAllocator allocator { 8, PAGE_SIZE / 8, pageAllocator, systemAllocator };

    void* ptr = allocator.allocate();

    EXPECT_TRUE(ptr != nullptr);
}