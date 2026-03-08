#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/memory/allocators/specific/pageBlockAllocator.h>
#include <dory/memory/allocators/specific/objectPoolAllocator.h>
#include <dory/memory/allocators/general/systemAllocator.h>
#include <dory/memory/allocators/general/segregationAllocator.h>
#include <dory/memory/genericMemoryResource.h>
#include <spdlog/fmt/bundled/format.h>
#include <array>

using namespace dory::memory;

#if DORY_PLATFORM_LINUX
#include <dory/profiling/metricsReader.h>

#include <sys/mman.h>
#include <immintrin.h>
#include <emmintrin.h> // For _mm_clflush
#include <sys/resource.h>

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

TEST(BlockAllocatorTests, pageResidency)
{
    using namespace std::chrono;

    constexpr std::size_t PAGE_SIZE = 4096;
    constexpr std::size_t PAGE_COUNT = 262144; //1Gb
    constexpr std::size_t INTS_IN_PAGE_COUNT = 4096 / sizeof(int);

    dory::profiling::ProcessMetrics processMetricsBefore;
    dory::profiling::ProcessMetrics processMetricsAfter;

    dory::profiling::MetricsReader::startMetricsRecording(processMetricsBefore);
    dory::profiling::MetricsReader::completeMetricsRecording(processMetricsBefore);

    auto allocator = allocators::specific::PageBlockAllocator(PAGE_SIZE, nullptr);

    void* ptr = allocator.allocateBlock({}, PAGE_COUNT);

    EXPECT_FALSE(ptr == nullptr);

    auto start = high_resolution_clock::now();

    dory::profiling::MetricsReader::startMetricsRecording(processMetricsAfter);

    for(std::size_t i = 0; i < PAGE_COUNT; ++i)
    {

        //write to the beginning of each page
        *((int*)ptr + INTS_IN_PAGE_COUNT * i) = -1;
    }

    dory::profiling::MetricsReader::completeMetricsRecording(processMetricsAfter);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;

    allocator.deallocateBlock(ptr, PAGE_COUNT);
    ptr = allocator.allocateBlock({}, PAGE_COUNT);

    //flush CPU caches
    for (size_t i = 0; i < PAGE_COUNT; ++i)
    {
        dory::data_structures::containers::lockfree::_mm_clflush((int*)ptr + INTS_IN_PAGE_COUNT * i);
    }

    start = high_resolution_clock::now();

    for(std::size_t i = 0; i < PAGE_COUNT; ++i)
    {
        //write to the beginning of each page
        *((int*)ptr + INTS_IN_PAGE_COUNT * i) = -1;
    }

    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;

    /*profile_residency(block.ptr, block.pagesCount * block.pageSize);
    BlockAllocator::commitPages(block, 1);
    profile_residency(block.ptr, block.pagesCount * block.pageSize);*/
}
#endif

TEST(FreeListAllocatorTests, simpleAllocation)
{
    constexpr std::size_t PAGE_SIZE = 4096;
    constexpr std::size_t SLOT_SIZE = 8;

    allocators::specific::PageBlockAllocator blockAllocator {PAGE_SIZE, nullptr};
    allocators::general::SystemAllocator systemAllocator { nullptr };
    allocators::specific::ObjectPoolAllocator freeListAllocator { SLOT_SIZE, (PAGE_SIZE / SLOT_SIZE) * 2, blockAllocator, systemAllocator };

    void* ptr = freeListAllocator.allocate();
    void* ptr2 = freeListAllocator.allocate();

    freeListAllocator.deallocate(ptr2);

    void* ptr3 = freeListAllocator.allocate();
}

using SegregationAllocatorType = allocators::general::SegregationAllocator<10, allocators::specific::PageBlockAllocator, allocators::general::SystemAllocator, allocators::general::SystemAllocator>;

TEST(SegregationAllocatorTests, simpleAllocation)
{
    constexpr std::size_t PAGE_SIZE = 4096;
    allocators::specific::PageBlockAllocator blockAllocator {PAGE_SIZE, nullptr};
    allocators::general::SystemAllocator systemAllocator { nullptr };

    std::array sizeClasses {
        allocators::general::MemorySizeClass{ 8, 1024 },
        allocators::general::MemorySizeClass{ 16, 1024 },
        allocators::general::MemorySizeClass{ 32, 1024 },
        allocators::general::MemorySizeClass{ 64, 1024 },
        allocators::general::MemorySizeClass{ 128, 1024 },
        allocators::general::MemorySizeClass{ 256, 1024 },
        allocators::general::MemorySizeClass{ 512, 1024 },
        allocators::general::MemorySizeClass{ 1024, 1024 },
        allocators::general::MemorySizeClass{ 2048, 1024 },
        allocators::general::MemorySizeClass{ 4096, 1024 }
    };

    SegregationAllocatorType segregationAllocator { blockAllocator, systemAllocator, systemAllocator, nullptr, sizeClasses };

    void* ptr1 = segregationAllocator.allocateBytes({}, 8);
    void* ptr2 = segregationAllocator.allocateBytes({}, 200);

    segregationAllocator.deallocateBytes(ptr1, 8);
    segregationAllocator.deallocateBytes(ptr2, 200);

    GenericMemoryResource<SegregationAllocatorType> memResource { segregationAllocator };
    const std::pmr::polymorphic_allocator<int> standardAllocator { &memResource };

    auto sptr1 = std::allocate_shared<int>(standardAllocator);
    auto sptr2 = std::allocate_shared<std::byte[8000]>(standardAllocator);

    *sptr1 = 10;
}