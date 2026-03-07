#pragma once

#include <atomic>
#include <dory/types.h>
#include <dory/macros/assert.h>
#include "resources/node.h"

namespace dory::data_structures::containers::lockfree
{
    template<typename T, typename TAllocator, LabelType AllocLabel>
    class TrackingList
    {
    private:
        using NodeType = resources::Node<T>;

        std::atomic<NodeType*> _head = nullptr;
        TAllocator& _allocator;

    public:
        explicit TrackingList(TAllocator& allocator):
            _allocator(allocator)
        {}

        ~TrackingList()
        {
            clear([](T&) {});
        }

        TrackingList(const TrackingList&) = delete;
        TrackingList& operator=(const TrackingList&) = delete;
        TrackingList(TrackingList&&) = delete;
        TrackingList& operator=(TrackingList&&) = delete;

        template<typename... TArgs>
        void push(TArgs&&... args)
        {
            NodeType* newNode = _allocator.template allocateObject<NodeType>(AllocLabel, std::forward<TArgs>(args)...);
            assert::release(newNode != nullptr, "TrackingList: cannot allocate new node");

            NodeType* currentHead = _head.load(std::memory_order_relaxed);
            newNode->next = currentHead;
            while(!_head.compare_exchange_weak(currentHead, newNode, std::memory_order_release, std::memory_order_relaxed))
            {
                newNode->next = currentHead;
            }
        }

        template<typename F>
        void clear(F&& fn)
        {
            NodeType* node = _head.exchange(nullptr, std::memory_order_acquire);
            while (node != nullptr)
            {
                NodeType* next = node->next;
                fn(node->data);
                _allocator.template deallocateObject<NodeType>(node);
                node = next;
            }
        }
    };
}