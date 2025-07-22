#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/memory/allocators/blockAllocator.h>

using namespace dory::memory;

void profile_residency(void* addr, size_t length) {
    long page_size = sysconf(_SC_PAGESIZE);
    size_t pages = (length + page_size - 1) / page_size;
    std::vector<unsigned char> vec(pages);

    if (mincore(addr, length, vec.data()) == 0) {
        for (size_t i = 0; i < pages; ++i) {
            std::cout << "Page " << i << ": " << ((vec[i] & 1) ? "Resident" : "Not Resident") << "\n";
        }
    }
}

TEST(BlockAllocatorTests, allocateOnePageOfMemory)
{
    constexpr std::size_t PAGE_SIZE = 4096;

    auto allocator = BlockAllocator(PAGE_SIZE);

    MemoryBlock block {};
    allocator.allocate(4, block); //1Gb

    EXPECT_FALSE(block.ptr == nullptr);

    profile_residency(block.ptr, block.pagesCount * block.pageSize);

    BlockAllocator::commitPages(block, 1);
    profile_residency(block.ptr, block.pagesCount * block.pageSize);

    BlockAllocator::commitPages(block, 2);
    profile_residency(block.ptr, block.pagesCount * block.pageSize);
}