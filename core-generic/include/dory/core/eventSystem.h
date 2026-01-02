#pragma once

#include <atomic>
#include <dory/data-structures/function.h>

namespace dory::core::event_system
{
    template<typename TKey = std::size_t>
    class EventHandlerKeyGenerator
    {
        static_assert(std::atomic<TKey>::is_always_lock_free, "HandlerKeyType atomic is not lock-free on this platform. Pick another numeric type.");

    public:
        using HandlerKeyType = TKey;

    private:
        std::atomic<HandlerKeyType> _currentKey { 1 }; //0 can be used as sentinel

    public:
        HandlerKeyType generate()
        {
            return _currentKey.fetch_add(1, std::memory_order::relaxed);
        }
    };

    template<typename TKeyGenerator, class... Ts>
    class Event
    {
    /*public:
        using KeyType = EventHandlerKeyType;
        using HandlerType = data_structures::function::Function<void(Ts...)>;

    private:
        std::atomic<KeyType> idCounter {}; //TODO: must be thread-safe

    protected:
        std::map<KeyType, std::shared_ptr<Callable<Ts...>>> handlers;
        //TODO: if the event used concurrently, it can lead to data race
        std::unordered_map<KeyType, std::shared_ptr<dory::generic::extension::ResourceHandle<HandlerType>>> _handlers;

    public:
        template<typename F>
        KeyType attachHandler(F&& function)
        {
            auto functor = std::forward<F>(function);
            return attachFunction(std::move(functor));
        }

        template<typename F>
        KeyType operator+=(F&& function)
        {
            return attachHandler(std::forward<F>(function));
        }

        KeyType attachHandler(std::function<void(Ts...)>&& functor)
        {
            return attachFunction(std::move(functor));
        }

        template<typename F>
        KeyType operator+=(std::function<void(Ts...)>&& functor)
        {
            return attachFunction(std::move(functor));
        }

        template<typename T>
        KeyType attachHandler(T* instance, void (T::* memberFunction)(Ts...))
        {
            return attachMemberFunction(instance, memberFunction);
        }

        KeyType attachHandlerLib(std::shared_ptr<dory::generic::extension::ResourceHandle<HandlerType>> handler)
        {
            KeyType key = getNewKey();
            _handlers[key] = handler;

            return key;
        }

        void detachHandler(KeyType handlerKey)
        {
            handlers.erase(handlerKey);
        }

    private:
        KeyType attachFunction(std::function<void(Ts...)>&& functor)
        {
            auto callable = std::make_shared<CallableFunction<Ts...>>(std::move(functor));
            return attachCallable(callable);
        }

        template<typename T>
        KeyType attachMemberFunction(T* instance, void (T::* memberFunction)(Ts...))
        {
            auto callable = std::make_shared<CallableMemberFunction<T, Ts...>>(instance, memberFunction);
            return attachCallable(callable);
        }

        KeyType attachCallable(std::shared_ptr<Callable<Ts...>> callable)
        {
            KeyType key = getNewKey();
            handlers.emplace(key, callable);

            return key;
        }

        KeyType getNewKey()
        {
            return idCounter++;
        }*/
    };
}