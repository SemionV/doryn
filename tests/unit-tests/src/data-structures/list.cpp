#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/fmt/fmt.h>
#include <allocatorBuilder.h>

#include <dory/data-structures/containers/lockfree/list.h>

TEST(SegmentedListTests, basic)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
    using AllocatorType = typename decltype(allocator)::element_type;

    auto list = dory::data_structures::containers::lockfree::SegmentedList<int, AllocatorType> { *allocator };
    const std::size_t i = list.append();
    int* slot = list.getSlot(i);

    EXPECT_TRUE(slot != nullptr);

    *slot = 1;
}