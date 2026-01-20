#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/fmt/fmt.h>
#include <allocatorBuilder.h>

#include <dory/data-structures/containers/lockfree/freeListArray.h>

TEST(FreeListTests, basic)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
    using AllocatorType = typename decltype(allocator)::element_type;

    auto list = dory::data_structures::containers::lockfree::freelist::FreeListArray<int, AllocatorType> { *allocator };
    const auto id1 = list.add(1);
    const auto id2 = list.add(2);

    EXPECT_EQ(list.size(), 2);

    EXPECT_EQ(list.get(id1), 1);
    EXPECT_EQ(list.get(id2), 2);

    list.forEach([](const int& value)
    {
        std::cout << "Item: " << value << std::endl;
    });

    list.retire(id1);

    list.remove(id1);
    EXPECT_EQ(list.size(), 1);

    const auto id3 = list.add(3);
    EXPECT_EQ(id3.index, id1.index); //item 3 has to be added to the previous slot of item 1
    list.forEach([](const int& value)
    {
        std::cout << "Item: " << value << std::endl;
    });

    const auto id4 = list.add(4);

    list.remove(id2);
    list.remove(id3);
    list.remove(id4);
    EXPECT_EQ(list.size(), 0);
}