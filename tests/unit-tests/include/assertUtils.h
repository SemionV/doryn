#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace dory::test_utilities
{
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
}