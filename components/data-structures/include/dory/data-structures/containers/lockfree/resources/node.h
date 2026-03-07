#pragma once

#include <utility>

namespace dory::data_structures::containers::lockfree::resources
{
    template<typename T>
    struct Node
    {
        T data;
        Node* next = nullptr;

        template<typename... TArgs>
        explicit Node(TArgs&&... args):
            data(std::forward<TArgs>(args)...)
        {}
    };
}
