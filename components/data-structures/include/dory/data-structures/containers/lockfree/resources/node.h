#pragma once

#include <utility>

namespace dory::data_structures::containers::lockfree::resources
{
    template<typename T>
    struct Node
    {
        T value;
        Node* next = nullptr;

        template <typename U>
        explicit Node(U&& v):
            value(std::forward<U>(v))
        {
        }
    };
}
