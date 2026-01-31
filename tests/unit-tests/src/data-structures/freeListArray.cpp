#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/fmt/fmt.h>
#include <allocatorBuilder.h>

#include <dory/data-structures/containers/lockfree/freeListArray.h>

//TODO: see why data value in the 0 slot is not initialized to 0
//TODO: find out about dynamic memory allocation happening after the initial allocation - probably it is allocation in retired list
//TODO: optimize method reclaim, taking in account that it will run isolated from other threads
//TODO: implement method clear
//TODO: implement a method to sort free list according to slot indexes(increase CPU cache efficiency by reducing random jumping in memory)

TEST(FreeListTests, basic)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
    using AllocatorType = typename decltype(allocator)::element_type;

    auto list = dory::data_structures::containers::lockfree::freelist::FreeListArray<int, AllocatorType> { *allocator };

    std::cout << "Free list" << std::endl;
    list.printFreeList();

    const auto id1 = list.add(1);
    const auto id2 = list.add(2);

    EXPECT_EQ(list.size(), 2);

    EXPECT_EQ(list.get(id1), 1);
    EXPECT_EQ(list.get(id2), 2);

    std::cout << "Add items 1, 2" << std::endl;
    list.print();
    std::cout << "Free list" << std::endl;
    list.printFreeList();

    list.retire(id1);
    list.retire(id1);

    list.reclaim();
    std::cout << "Delete item 1" << std::endl;
    list.print();
    std::cout << "Free list" << std::endl;
    list.printFreeList();
    EXPECT_EQ(list.size(), 1);

    const auto id3 = list.add(3);
    EXPECT_EQ(id3.index, id1.index); //item 3 has to be added to the previous slot of item 1
    std::cout << "Add item 3" << std::endl;
    list.print();
    std::cout << "Free list" << std::endl;
    list.printFreeList();

    const auto id4 = list.add(4);
    std::cout << "Add item 4" << std::endl;
    list.print();
    std::cout << "Free list" << std::endl;
    list.printFreeList();

    list.retire(id2);
    list.retire(id3);
    list.retire(id4);
    list.reclaim();
    std::cout << "Delete items 2, 3, 4" << std::endl;
    list.print();
    std::cout << "Free list" << std::endl;
    list.printFreeList();
    EXPECT_EQ(list.size(), 0);
}