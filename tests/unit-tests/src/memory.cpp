#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/memory/allocators/blockAllocator.h>
#include <dory/profiling/metricsReader.h>

#include <emmintrin.h> // For _mm_clflush
#include <sys/resource.h>

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

    auto allocator = BlockAllocator(PAGE_SIZE);

    MemoryBlock block {};
    allocator.allocate(PAGE_COUNT, block);

    EXPECT_FALSE(block.ptr == nullptr);

    auto start = high_resolution_clock::now();

    dory::profiling::MetricsReader::startMetricsRecording(processMetricsAfter);

    for(std::size_t i = 0; i < PAGE_COUNT; ++i)
    {
        //write to the beginning of each page
        *((int*)block.ptr + INTS_IN_PAGE_COUNT * i) = -1;
    }

    dory::profiling::MetricsReader::completeMetricsRecording(processMetricsAfter);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;

    allocator.deallocate(block);
    allocator.allocate(PAGE_COUNT, block);
    BlockAllocator::commitPages(block, PAGE_COUNT);

    //flush CPU caches
    for (size_t i = 0; i < PAGE_COUNT; ++i)
    {
        _mm_clflush((int*)block.ptr + INTS_IN_PAGE_COUNT * i);
    }

    start = high_resolution_clock::now();

    for(std::size_t i = 0; i < PAGE_COUNT; ++i)
    {
        //write to the beginning of each page
        *((int*)block.ptr + INTS_IN_PAGE_COUNT * i) = -1;
    }

    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;

    /*profile_residency(block.ptr, block.pagesCount * block.pageSize);
    BlockAllocator::commitPages(block, 1);
    profile_residency(block.ptr, block.pagesCount * block.pageSize);*/
}