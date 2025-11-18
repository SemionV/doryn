#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/fmt/fmt.h>

#include "dory/memory/allocators/segregationAllocator.h"
#include "dory/memory/allocators/standardAllocator.h"
#include "dory/memory/allocators/systemAllocator.h"
#include <dory/containers/string.h>
#include <dory/containers/list.h>

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

    using DoryString = dory::containers::BasicString<char, std::char_traits<char>, SegregationAllocatorType>;

    DoryString str { "Hello", segregationAllocator };

    const dory::containers::CRC32Table table = dory::containers::CRC32::generateTable();

    str.append(" World!");
    std::cout << str.length() << ": " << str.data() << std::endl;
    std::cout << "CRC32: " << str.crc32(table) << std::endl;

    DoryString  str2 {"Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.", segregationAllocator};
    std::cout << str2.length() << ": " << str2.data() << std::endl;
    std::cout << "CRC32: " << str2.crc32(table) << std::endl;

    str2.append(" At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.");
    std::cout << str2.length() << ": " << str2.data() << std::endl;
    std::cout << "CRC32: " << str2.crc32(table) << std::endl;

    std::cout << "std::string size: " << sizeof(std::string) << std::endl;
    std::cout << "dory::containers::BasicString size: " << sizeof(dory::containers::BasicString<char, std::char_traits<char>, SegregationAllocatorType>) << std::endl;
}