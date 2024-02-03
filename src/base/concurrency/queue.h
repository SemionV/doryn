#pragma once

#include "base/dependencies.h"

namespace dory::concurrency
{
    template<typename T>
    class Queue
    {
    private:
        struct Node
        {
            std::optional<T> data;
            std::unique_ptr<Node> next;
        };

        std::mutex headMutex;
        std::mutex tailMutex;
        std::condition_variable pushItemCondition;

        std::unique_ptr<Node> head;
        Node* tail;

    public:
        Queue():
            head(new Node()),
            tail(head.get())
        {}

        Queue(const Queue& other) = delete;
        Queue& operator=(const Queue& other) = delete;

        std::optional<T> tryPop();
        std::optional<T> waitAndPop();
        bool isEmpty();
        void push(T value);
        void push(T&& value);
    };

    template<typename T, std::size_t UpperBound>
    class BoundedQueue
    {
    private:
        std::array<std::optional<T>, UpperBound> data;
        std::size_t headIndex = 0;
        std::size_t tailIndex = 0;

        void incrementIndex(std::size_t& index)
        {
            if(index < UpperBound - 1)
            {
                ++index;
            }
            else
            {
                index = 0;
            }
        }

    public:
        bool push(T value)
        {
            if(data[tailIndex].has_value())
            {
                return false;
            }

            data[tailIndex] = value;
            incrementIndex(tailIndex);

            return true;
        }

        std::optional<T> pop()
        {
            if(data[headIndex].has_value())
            {
                auto value = data[headIndex];
                data[headIndex].reset();
                incrementIndex(headIndex);

                return value;
            }

            return {};
        }
    };

    template<typename T, std::size_t UpperBound>
    class BoundedQueueConcurrent
    {
    private:
        struct Item
        {
            std::optional<T> value;
            std::mutex mutex;
        };

        std::array<Item, UpperBound> data;
        std::size_t headIndex = 0;
        std::size_t tailIndex = 0;

        std::mutex headIndexMutex;
        std::mutex tailIndexMutex;

        void incrementIndex(std::size_t& index)
        {
            if(index < UpperBound - 1)
            {
                ++index;
            }
            else
            {
                index = 0;
            }
        }

    public:
        bool push(T value)
        {
            std::unique_lock<std::mutex> tailIndexLock(tailIndexMutex);
            auto& item = data[tailIndex];
            std::lock_guard<std::mutex> cellLock(item.mutex);
            if(!item.value.has_value())
            {
                incrementIndex(tailIndex);
                tailIndexLock.unlock();

                item.value = value;

                return true;
            }

            return false;
        }

        std::optional<T> pop()
        {
            std::unique_lock<std::mutex> headIndexLock(headIndexMutex);
            auto& item = data[headIndex];
            std::lock_guard<std::mutex> cellLock(item.mutex);
            if(item.value.has_value())
            {
                incrementIndex(headIndex);
                headIndexLock.unlock();

                auto value = item.value.value();
                item.value.reset();

                return value;
            }

            return {};
        }
    };
}