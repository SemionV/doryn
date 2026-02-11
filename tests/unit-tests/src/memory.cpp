#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/memory/allocators/pageAllocator.h>
#include <dory/memory/allocators/freeListAllocator.h>
#include <dory/memory/allocators/systemAllocator.h>
#include <dory/memory/allocators/segregationAllocator.h>
#include <dory/memory/allocators/standardAllocator.h>
#include <dory/memory/allocators/objectPoolAllocator.h>
#include <dory/memory/profilers/objectPoolAllocationProfiler.h>
#include <spdlog/fmt/bundled/format.h>
#include <array>

using namespace dory::memory;

#if DORY_PLATFORM_LINUX
#include <dory/profiling/metricsReader.h>

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

    auto allocator = PageAllocator(PAGE_SIZE, nullptr);

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
    PageAllocator::commitPages(block, PAGE_COUNT);

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
#endif

TEST(FreeListAllocatorTests, simpleAllocation)
{
    constexpr std::size_t PAGE_SIZE = 4096;
    constexpr std::size_t SLOT_SIZE = 8;

    PageAllocator blockAllocator {PAGE_SIZE, nullptr};
    SystemAllocator systemAllocator;
    FreeListAllocator freeListAllocator { SLOT_SIZE, (PAGE_SIZE / SLOT_SIZE) * 2, blockAllocator, systemAllocator };

    void* ptr = freeListAllocator.allocate();
    void* ptr2 = freeListAllocator.allocate();

    freeListAllocator.deallocate(ptr2);

    void* ptr3 = freeListAllocator.allocate();
}

class AllocProfiler
{
public:
    void traceSlotAlloc(void* ptr, std::size_t size, std::size_t slotSize, std::size_t classIndex)
    {
       std::cout << fmt::format("Slot allocated: size [{0}], slot[{1}], class[{2}], ptr[{3}]", size, slotSize, classIndex, ptr) << std::endl;
    }

    void traceSlotFree(void* ptr, std::size_t slotSize, std::size_t classIndex)
    {
        std::cout << fmt::format("Slot deallocated: slot[{0}], class[{1}], ptr[{2}]", slotSize, classIndex, ptr) << std::endl;
    }

    void traceLargeAlloc(void* ptr, std::size_t size)
    {
        std::cout << fmt::format("Large object allocated: size [{0}], ptr[{1}]", size, ptr) << std::endl;
    }

    void traceLargeFree(void* ptr, std::size_t size)
    {
        std::cout << fmt::format("Large object deallocated: ptr[{0}], size[{1}]", ptr, size) << std::endl;
    }
};

using SegregationAllocatorType = SegregationAllocator<10, PageAllocator, SystemAllocator, SystemAllocator, AllocProfiler>;

template<typename T>
using StandardAllocatorType = StandardAllocator<T, SegregationAllocatorType>;

TEST(SegregationAllocatorTests, simpleAllocation)
{
    constexpr std::size_t PAGE_SIZE = 4096;
    PageAllocator blockAllocator {PAGE_SIZE, nullptr};
    SystemAllocator systemAllocator;

    std::array sizeClasses {
        MemorySizeClass{ 8, 1024 },
        MemorySizeClass{ 16, 1024 },
        MemorySizeClass{ 32, 1024 },
        MemorySizeClass{ 64, 1024 },
        MemorySizeClass{ 128, 1024 },
        MemorySizeClass{ 256, 1024 },
        MemorySizeClass{ 512, 1024 },
        MemorySizeClass{ 1024, 1024 },
        MemorySizeClass{ 2048, 1024 },
        MemorySizeClass{ 4096, 1024 }
    };

    AllocProfiler profiler;
    SegregationAllocatorType segregationAllocator { "testSegAlloc", blockAllocator, systemAllocator, systemAllocator, profiler, sizeClasses };

    void* ptr1 = segregationAllocator.allocate(8);
    void* ptr2 = segregationAllocator.allocate(200);

    segregationAllocator.deallocate(ptr1);
    segregationAllocator.deallocate(ptr2);

    StandardAllocatorType<int> standardAllocator { segregationAllocator };

    auto sptr1 = std::allocate_shared<int>(standardAllocator);
    auto sptr2 = std::allocate_shared<std::byte[8000]>(standardAllocator);

    *sptr1 = 10;
}

TEST(ObjectPoolAllocatorTests, chunkAllocation)
{
    constexpr std::size_t PAGE_SIZE = 4096;
    PageAllocator blockAllocator {PAGE_SIZE, nullptr};
    SystemAllocator systemAllocator;
    profilers::ObjectPoolAllocationProfiler profiler;

    allocators::ObjectPoolAllocator<int, PageAllocator, SystemAllocator, 1024> objectPool { blockAllocator, systemAllocator, &profiler };

    EXPECT_TRUE(objectPool.empty());

    objectPool.reserve();

    EXPECT_EQ(profiler.chunksAllocated, 1);
    EXPECT_EQ(profiler.memoryAllocated, PAGE_SIZE);

    EXPECT_FALSE(objectPool.empty());
}