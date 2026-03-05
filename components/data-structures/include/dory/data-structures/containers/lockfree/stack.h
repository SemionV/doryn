#pragma once

#include <atomic>
#include <dory/types.h>
#include <dory/macros/assert.h>

namespace dory::data_structures::containers::lockfree
{
    template<typename T, typename TAllocator, LabelType AllocLabel>
    class Stack
    {
    private:
        struct Node
        {
            T data;
            Node* next;
        };

        std::atomic<Node*> _head = nullptr;
        TAllocator& _allocator;

    public:
        explicit Stack(TAllocator& allocator):
            _allocator(allocator)
        {}

        void push(const T& data)
        {
            Node* newNode = _allocator.template allocateObject<Node>(AllocLabel);
            assert::release(newNode != nullptr, "Stack: cannot allocate new node");

            newNode->data = data;

            Node* currentHead = _head.load(std::memory_order_relaxed);
            newNode->next = currentHead;
            while(!_head.compare_exchange_weak(currentHead, newNode, std::memory_order_release, std::memory_order_relaxed))
            {
                newNode->next = currentHead;
            }
        }

        bool tryPop(T& destination)
        {
            while(true)
            {
                Node* currentHead = _head.load(std::memory_order_relaxed);
                if(currentHead == nullptr)
                    return false;

                if(_head.compare_exchange_weak(currentHead, currentHead->next, std::memory_order_acquire, std::memory_order_relaxed))
                {
                    destination = std::move(currentHead->data);
                    return true;
                }
            }
        }
    };
}