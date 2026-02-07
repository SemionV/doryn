#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/fmt/fmt.h>
#include <allocatorBuilder.h>

#include <dory/data-structures/containers/lockfree/freeListArray.h>

#include "assertUtils.h"

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
    list.sortFreeList();
    list.printFreeList();
    EXPECT_EQ(list.size(), 0);
}

TEST(FreeListTests, sorting)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
    using AllocatorType = typename decltype(allocator)::element_type;
    using ListType = dory::data_structures::containers::lockfree::freelist::FreeListArray<int, AllocatorType, 16>;
    using SlotType = ListType::SlotType;

    auto list = ListType { *allocator };
    auto freeListState = dory::data_structures::containers::lockfree::freelist::FreeListState<ListType::SlotIndexType, ListType::UNDEFINED_HEAD_INDEX> {};

    for(std::size_t i = 0; i != 11; ++i)
    {
        list.reserveSlot();
    }

    freeListState.head.store(10);

    SlotType* slot = list.getSlot(10);
    slot->nextSlot.store(3);

    slot = list.getSlot(3);
    slot->nextSlot.store(8);

    slot = list.getSlot(8);
    slot->nextSlot.store(2);

    slot = list.getSlot(2);
    slot->nextSlot.store(7);

    slot = list.getSlot(7);
    slot->nextSlot.store(ListType::UNDEFINED_HEAD_INDEX);

    dory::data_structures::containers::lockfree::freelist::sort(freeListState, list);

    std::vector<ListType::SlotIndexType> freeListIndices;
    ListType::SlotIndexType index = freeListState.head.load();
    while(index != ListType::UNDEFINED_HEAD_INDEX)
    {
        freeListIndices.push_back(index);
        slot = list.getSlot(index);
        index = slot->nextSlot.load();
    }

    dory::test_utilities::assertList(freeListIndices, { 2, 3, 7, 8, 10 });
}