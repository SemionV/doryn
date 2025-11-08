#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/fmt/fmt.h>

#include "dory/memory/allocators/segregationAllocator.h"
#include "dory/memory/allocators/standardAllocator.h"
#include "dory/memory/allocators/systemAllocator.h"
#include <dory/containers/string.h>

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

    void traceLargeFree(void* ptr)
    {
        std::cout << fmt::format("Large object deallocated: ptr[{0}]", ptr) << std::endl;
    }
};

using SegregationAllocatorType = dory::memory::SegregationAllocator<10, dory::memory::PageAllocator, dory::memory::SystemAllocator, dory::memory::SystemAllocator, AllocProfiler>;

template<typename T>
using StandardAllocatorType = dory::memory::StandardAllocator<T, SegregationAllocatorType>;

TEST(BasicStringTests, simpleTest)
{
    constexpr std::size_t PAGE_SIZE = 4096;
    dory::memory::PageAllocator blockAllocator {PAGE_SIZE};
    dory::memory::SystemAllocator systemAllocator;

    std::array sizeClasses {
        dory::memory::MemorySizeClass{ 8, 1024 },
        dory::memory::MemorySizeClass{ 16, 1024 },
        dory::memory::MemorySizeClass{ 32, 1024 },
        dory::memory::MemorySizeClass{ 64, 1024 },
        dory::memory::MemorySizeClass{ 128, 1024 },
        dory::memory::MemorySizeClass{ 256, 1024 },
        dory::memory::MemorySizeClass{ 512, 1024 },
        dory::memory::MemorySizeClass{ 1024, 1024 },
        dory::memory::MemorySizeClass{ 2048, 1024 },
        dory::memory::MemorySizeClass{ 4096, 1024 }
    };

    AllocProfiler profiler;
    SegregationAllocatorType segregationAllocator { "testSegAlloc", blockAllocator, systemAllocator, systemAllocator, profiler, sizeClasses };

    dory::containers::BasicString<char, std::char_traits<char>, SegregationAllocatorType> str { "Hello", segregationAllocator };

    str.append(" World!");

    std::cout << str.data() << std::endl;
}