#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/fmt/fmt.h>
#include <allocatorBuilder.h>
#include <assertUtils.h>

#include "dory/memory/allocators/segregationAllocator.h"
#include <dory/data-structures/containers/string.h>
#include <dory/data-structures/containers/list.h>
#include <dory/data-structures/containers/deque.h>
#include <dory/data-structures/containers/hashMap.h>

using SegregationAllocatorType = dory::test_utilities::AllocatorBuilder<>::SegregationAllocatorType;

using DoryString = dory::data_structures::containers::BasicString<char, std::char_traits<char>, SegregationAllocatorType>;

template<typename T>
using DoryList = dory::data_structures::containers::BasicList<T, SegregationAllocatorType>;

template<typename T, std::size_t BlockSize = 64>
using DoryDeque = dory::data_structures::containers::BasicDeque<T, SegregationAllocatorType, BlockSize>;

namespace std
{
    template<class CharT, class Traits, class Alloc>
    struct hash<dory::data_structures::containers::BasicString<CharT, Traits, Alloc>>
    {
        size_t operator()(dory::data_structures::containers::BasicString<CharT, Traits, Alloc> const& s) const noexcept
        {
            std::basic_string_view<CharT, Traits> sv{ s.data(), s.size() };
            return std::hash<std::basic_string_view<CharT, Traits>>{}(sv);
        }
    };
}

template<typename TKey, typename TValue>
using DoryHashMap = dory::data_structures::containers::hashMap::HashMap<TKey, TValue, SegregationAllocatorType>;


TEST(BasicStringTests, simpleTest)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();

    DoryString str { "Hello", *allocator };

    const dory::data_structures::containers::CRC32Table table = dory::data_structures::containers::CRC32::generateTable();

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
    std::cout << "dory::containers::BasicString size: " << sizeof(dory::data_structures::containers::BasicString<char, std::char_traits<char>, SegregationAllocatorType>) << std::endl;
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

TEST(BasicListTests, simpleTest)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
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

    dory::test_utilities::assertList(list, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    printList(list);

    list.pop_back();
    list.pop_back();
    list.pop_back();
    list.pop_back();
    list.pop_back();

    list.emplace(list.begin() + 2, 11);

    dory::test_utilities::assertList(list, {1, 2, 11, 3, 4, 5});
    printList(list);
}

TEST(BasicDequeTests, simpleTest)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
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

    dory::test_utilities::assertList(list, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
    printList(list);

    EXPECT_EQ(list[1], 2);

    list.clear();
    dory::test_utilities::assertList<decltype(list), int>(list, {});
}

TEST(BasicDequeTests, popTest)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
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
    dory::test_utilities::assertList(list, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    list.pop_front();
    dory::test_utilities::assertList(list, {2, 3, 4, 5, 6, 7, 8, 9, 10});

    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.pop_front();
    list.pop_front();
    dory::test_utilities::assertList<decltype(list), int>(list, {});
}

TEST(BasicHashMapTests, simpleTest)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();

    const auto key1 = DoryString{ "key1", *allocator };
    const auto key2 = DoryString{ "key2", *allocator };
    DoryHashMap<DoryString, int> map { *allocator,
    {
        { key1, 1 },
        { key2, 2 }
    }};

    EXPECT_EQ(map.size(), 2);

    const auto result = map.find(key1);
    EXPECT_TRUE(result != map.end());
    EXPECT_EQ(result->first, key1);
    EXPECT_EQ(result->second, 1);
    EXPECT_EQ(map[key1], 1);

    const auto result2 = map.find(key2);
    EXPECT_TRUE(result2 != map.end());
    EXPECT_EQ(result2->first, key2);
    EXPECT_EQ(result2->second, 2);
    EXPECT_EQ(map[key2], 2);
}

TEST(BasicHashMapTests, eraseTest)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();

    const auto key1 = DoryString{ "key1", *allocator };
    const auto key2 = DoryString{ "key2", *allocator };
    DoryHashMap<DoryString, int> map { *allocator,
    {
        { key1, 1 },
        { key2, 2 }
    }};

    EXPECT_EQ(map.size(), 2);

    map.erase(key1);

    EXPECT_EQ(map.size(), 1);

    auto result = map.find(key1);
    EXPECT_EQ(result, map.end());
    result = map.find(key2);
    EXPECT_NE(result, map.end());
}

TEST(BasicHashMapTests, clearTest)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();

    const auto key1 = DoryString{ "key1", *allocator };
    const auto key2 = DoryString{ "key2", *allocator };
    DoryHashMap<DoryString, int> map { *allocator,
    {
        { key1, 1 },
        { key2, 2 }
    }};

    EXPECT_EQ(map.size(), 2);

    map.clear();

    EXPECT_EQ(map.size(), 0);
}