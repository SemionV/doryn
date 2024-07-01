#pragma once

#include "../dependencies.h"

namespace dory::concurrency
{
    template<typename T, std::size_t UpperBound>
    class BoundedQueue
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

        std::condition_variable pushCondition;
        std::condition_variable popCondition;

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

        std::optional<T> popHead(std::unique_lock<std::mutex>& headIndexLock)
        {
            auto& item = data[headIndex];
            std::lock_guard<std::mutex> cellLock(item.mutex);
            if(item.value.has_value())
            {
                incrementIndex(headIndex);
                headIndexLock.unlock();

                auto value = item.value.value();
                item.value.reset();
                popCondition.notify_one();

                return value;
            }

            return {};
        }

        bool pushTail(T value, std::unique_lock<std::mutex>& tailIndexLock)
        {
            auto& item = data[tailIndex];
            std::lock_guard<std::mutex> cellLock(item.mutex);
            if(!item.value.has_value())
            {
                incrementIndex(tailIndex);
                tailIndexLock.unlock();

                item.value = value;
                pushCondition.notify_one();

                return true;
            }

            return false;
        }

        bool isEmptyLocked(const std::unique_lock<std::mutex>& mutex)
        {
            auto& item = data[headIndex];
            std::lock_guard<std::mutex> cellLock(item.mutex);

            return !item.value.has_value();
        }

        bool isFullLocked(const std::unique_lock<std::mutex>& mutex)
        {
            auto& item = data[tailIndex];
            std::lock_guard<std::mutex> cellLock(item.mutex);

            return item.value.has_value();
        }

    public:
        bool push(T value)
        {
            std::unique_lock<std::mutex> tailIndexLock(tailIndexMutex);
            return pushTail(value, tailIndexLock);
        }

        bool waitAndPush(T value)
        {
            std::unique_lock<std::mutex> tailIndexLock(tailIndexMutex);
            popCondition.wait(tailIndexLock, [this, &tailIndexLock](){return !isFullLocked(tailIndexLock);});
            return pushTail(value, tailIndexLock);
        }

        std::optional<T> pop()
        {
            std::unique_lock<std::mutex> headIndexLock(headIndexMutex);
            return popHead(headIndexLock);
        }

        std::optional<T> waitAndPop()
        {
            auto headIndexLock = std::unique_lock<std::mutex>(headIndexMutex);
            pushCondition.wait(headIndexLock, [this, &headIndexLock](){return !isEmptyLocked(headIndexLock);});
            return popHead(headIndexLock);
        }

        bool isEmpty()
        {
            std::unique_lock<std::mutex> headIndexLock(headIndexMutex);
            return isEmptyLocked(headIndexLock);
        }

        bool isFull()
        {
            std::unique_lock<std::mutex> tailIndexLock(tailIndexMutex);
            return isFullLocked(tailIndexLock);
        }
    };
}