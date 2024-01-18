#pragma once

#include "base/dependencies.h"
#include "log.h"

namespace dory::concurrency::messaging
{
    template<std::size_t milliseconds>
    class TimeLimitiedConditionVariable
    {
    private:
        std::condition_variable conditionVariable;

    public:
        void notify_one()
        {
            conditionVariable.notify_one();
        }

        template<typename TLock, typename F>
        auto wait(TLock&& lock, F&& functor)
        {
            return conditionVariable.wait_for(std::forward<TLock>(lock), std::chrono::milliseconds(milliseconds), std::forward<F>(functor));
        }
    };

    template<typename T, typename TLogPolicy = logging::EmptyLogPolicy, typename TConditionVariable = std::condition_variable>
    class MessageQueue
    {
        std::mutex queueMutex;
        TConditionVariable queueCondition;
        std::queue<T> queue;
        TLogPolicy::TLogger* logger;
    public:
        explicit MessageQueue(TLogPolicy::TLogger* logger):
                logger(logger)
        {}

        MessageQueue():
            logger(nullptr)
        {}

        void pushMessage(T&& message)
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            queue.emplace(std::forward<T>(message));
            queueCondition.notify_one();
        }
        T waitForMessage()
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [&]
            {
                return !queue.empty();
            });

            auto message = queue.front();
            queue.pop();
            return message;
        }
    };
}