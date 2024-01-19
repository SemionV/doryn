#pragma once

#include "base/dependencies.h"
#include "log.h"

namespace dory::concurrency::messaging
{
    template<typename T>
    class SingleElementQueue
    {
    private:
        std::optional<T> container = {};

    public:
        decltype(auto) emplace(T&& element)
        {
            return container.emplace(std::forward<T>(element));
        }

        decltype(auto) front()
        {
            return container.value();
        }

        void pop()
        {
            container.reset();
        }

        bool empty()
        {
            return !container.has_value();
        }
    };

    template<typename TMessage>
    using QueueType = std::queue<TMessage, std::deque<TMessage>>;

    template<typename TQueue, typename TMessage>
    class MessageSender
    {
    private:
        template<template<typename T> class TQ, typename... Ts>
        friend class MessageSenderHub;

        TQueue& queue;
        std::mutex& senderMutex;
        std::condition_variable& sendCondition;

        void sendMessage(TMessage&& message)
        {
            std::lock_guard<std::mutex> lock(senderMutex);
            queue.emplace(std::forward<TMessage>(message));
            sendCondition.notify_one();
        }

    public:
        explicit MessageSender(TQueue& queue, std::mutex& senderMutex, std::condition_variable& sendCondition):
                queue(queue),
                senderMutex(senderMutex),
                sendCondition(sendCondition)
        {}
    };

    template<template<typename T> class TQueue, typename... Ts>
    class MessageSenderHub: public MessageSender<TQueue<Ts>, Ts>...
    {
    public:
        explicit MessageSenderHub(TQueue<Ts>&... queue, std::mutex& senderMutex, std::condition_variable& sendCondition):
                MessageSender<TQueue<Ts>, Ts>(queue, senderMutex, sendCondition)...
        {}

        template<typename T>
        void send(T&& message)
        {
            MessageSender<TQueue<T>, T>::sendMessage(std::forward<T>(message));
        }
    };

    template<typename TQueue, typename TMessage>
    class MessageReciever
    {
    private:
        template<template<typename T> class TQ, typename... Ts>
        friend class MessageRecieverHub;

        TQueue queue;
        std::function<bool(TMessage&&)> messageHandler;

        bool notifySubscriber()
        {
            bool stop = false;

            while(!queue.empty())
            {
                auto message = queue.front();
                queue.pop();
                if(messageHandler)
                {
                    if(!messageHandler(std::move(message)))
                    {
                        stop = true;
                    }
                }
            }

            return !stop;
        }

        template<typename F>
        void subscribeHandler(F&& handler)
        {
            messageHandler = std::forward<F>(handler);
        }

        bool checkInbox()
        {
            return !queue.empty();
        }
    };

    template<template<typename T> class TQueue, typename... Ts>
    class MessageRecieverHub: public MessageReciever<TQueue<Ts>, Ts>...
    {
    private:
        std::mutex recieverMutex;
        std::condition_variable recieveCondition;

    public:
        MessageSenderHub<TQueue, Ts...> getSender()
        {
            return MessageSenderHub<TQueue, Ts...>(MessageReciever<TQueue<Ts>, Ts>::queue..., recieverMutex, recieveCondition);
        }

        template<typename T, typename F>
        void subscribe(F&& handler)
        {
            MessageReciever<TQueue<T>, T>::subscribeHandler(std::forward<F>(handler));
        }

        bool wait()
        {
            std::unique_lock<std::mutex> lock(recieverMutex);
            recieveCondition.wait(lock, [this]
            {
                return (MessageReciever<TQueue<Ts>, Ts>::checkInbox() || ...);
            });

            return (MessageReciever<TQueue<Ts>, Ts>::notifySubscriber() && ...);
        }
    };

    template<typename... Ts>
    using SingleMessageRecieverHub = MessageRecieverHub<SingleElementQueue, Ts...>;

    template<typename... Ts>
    using MultiMessageRecieverHub = MessageRecieverHub<QueueType, Ts...>;
}