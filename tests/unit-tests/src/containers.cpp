#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/fmt/fmt.h>

#include "dory/memory/allocators/segregationAllocator.h"
#include "dory/memory/allocators/standardAllocator.h"
#include "dory/memory/allocators/systemAllocator.h"
#include <dory/containers/string.h>
#include <dory/containers/list.h>
#include <dory/containers/deque.h>
#include <dory/containers/hashMap.h>

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

using DoryString = dory::containers::BasicString<char, std::char_traits<char>, SegregationAllocatorType>;

template<typename T>
using DoryList = dory::containers::BasicList<T, SegregationAllocatorType>;

template<typename T, std::size_t BlockSize = 64>
using DoryDeque = dory::containers::BasicDeque<T, SegregationAllocatorType, BlockSize>;

namespace std
{
    template<class CharT, class Traits, class Alloc>
    struct hash<dory::containers::BasicString<CharT, Traits, Alloc>>
    {
        size_t operator()(dory::containers::BasicString<CharT, Traits, Alloc> const& s) const noexcept
        {
            std::basic_string_view<CharT, Traits> sv{ s.data(), s.size() };
            return std::hash<std::basic_string_view<CharT, Traits>>{}(sv);
        }
    };
}

template<typename TKey, typename TValue>
using DoryHashMap = dory::containers::hashMap::HashMap<TKey, TValue, SegregationAllocatorType>;

std::shared_ptr<SegregationAllocatorType> buildAllocator()
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

    return std::make_shared<SegregationAllocatorType>("testSegAlloc", blockAllocator, systemAllocator, systemAllocator, profiler, sizeClasses);
}

TEST(BasicStringTests, simpleTest)
{
    const auto allocator = buildAllocator();

    DoryString str { "Hello", *allocator };

    const dory::containers::CRC32Table table = dory::containers::CRC32::generateTable();

    str.append(" World!");
    std::cout << str.length() << ": " << str.data() << std::endl;
    std::cout << "CRC32: " << str.crc32(table) << std::endl;

    DoryString  str2 {"Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.", *allocator};
    std::cout << str2.length() << ": " << str2.data() << std::endl;
    std::cout << "CRC32: " << str2.crc32(table) << std::endl;

    str2.append(" At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.");
    std::cout << str2.length() << ": " << str2.data() << std::endl;
    std::cout << "CRC32: " << str2.crc32(table) << std::endl;

    std::cout << "std::string size: " << sizeof(std::string) << std::endl;
    std::cout << "dory::containers::BasicString size: " << sizeof(dory::containers::BasicString<char, std::char_traits<char>, SegregationAllocatorType>) << std::endl;
}

template<typename TList>
void printList(const TList& list)
{
    std::size_t i = 0;
    for(const auto& value : list)
    {
        std::cout << "item " << i++ << ": " << value << std::endl;
    }
}

template<typename TList, typename T>
void assertList(const TList& list, std::initializer_list<T> expected)
{
    EXPECT_EQ(list.size(), expected.size());

    std::size_t i = 0;
    for (const auto& val : expected)
    {
        EXPECT_EQ(list.at(i), val);
        ++i;
    }
}

TEST(BasicListTests, simpleTest)
{
    const auto allocator = buildAllocator();
    DoryList<int> list {*allocator};

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);
    list.push_back(6);
    list.push_back(7);
    list.push_back(8);
    list.push_back(9);
    list.push_back(10);

    assertList(list, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    printList(list);

    list.pop_back();
    list.pop_back();
    list.pop_back();
    list.pop_back();
    list.pop_back();

    list.emplace(list.begin() + 2, 11);

    assertList(list, {1, 2, 11, 3, 4, 5});
    printList(list);
}

TEST(BasicDequeTests, simpleTest)
{
    const auto allocator = buildAllocator();
    DoryDeque<int, 4> list {*allocator};

    list.push_front(5);
    list.push_front(4);
    list.push_front(3);
    list.push_front(2);
    list.push_front(1);
    list.push_back(6);
    list.push_back(7);
    list.push_back(8);
    list.push_back(9);
    list.push_back(10);

    int val = 11;
    list.push_back(val);

    assertList(list, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
    printList(list);

    EXPECT_EQ(list[1], 2);

    list.clear();
    assertList<decltype(list), int>(list, {});
}

TEST(BasicDequeTests, popTest)
{
    const auto allocator = buildAllocator();
    DoryDeque<int, 4> list {*allocator};

    list.push_front(5);
    list.push_front(4);
    list.push_front(3);
    list.push_front(2);
    list.push_front(1);
    list.push_back(6);
    list.push_back(7);
    list.push_back(8);
    list.push_back(9);
    list.push_back(10);

    int val = 11;
    list.push_back(val);

    list.pop_back();
    assertList(list, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    list.pop_front();
    assertList(list, {2, 3, 4, 5, 6, 7, 8, 9, 10});

    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.pop_front();
    assertList<decltype(list), int>(list, {});
}

TEST(BasicHashMapTests, simpleTest)
{
    const auto allocator = buildAllocator();
    DoryHashMap<DoryString, int> map { *allocator };

    const auto key1 = DoryString{ "key1", *allocator };
    map.insert({ key1, 1 });

    const auto result = map.find(key1);
    EXPECT_TRUE(result != map.end());
    EXPECT_EQ(result->first, key1);
    EXPECT_EQ(result->second, 1);
}