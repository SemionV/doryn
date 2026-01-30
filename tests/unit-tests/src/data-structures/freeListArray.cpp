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

    std::cout << "Added items 1, 2" << std::endl;
    list.forEach([](const int& value)
    {
        std::cout << "Item: " << value << std::endl;
    });

    list.retire(id1);
    list.retire(id1);

    list.reclaim();
    EXPECT_EQ(list.size(), 1);

    const auto id3 = list.add(3);
    EXPECT_EQ(id3.index, id1.index); //item 3 has to be added to the previous slot of item 1
    std::cout << "Removed item 1 and added item 3" << std::endl;
    list.forEach([](const int& value)
    {
        std::cout << "Item: " << value << std::endl;
    });

    const auto id4 = list.add(4);
    std::cout << "Added item 4" << std::endl;
    list.forEach([](const int& value)
    {
        std::cout << "Item: " << value << std::endl;
    });

    list.retire(id2);
    list.retire(id3);
    list.retire(id4);
    list.reclaim();
    EXPECT_EQ(list.size(), 0);
}