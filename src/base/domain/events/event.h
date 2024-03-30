#pragma once

#include "base/dependencies.h"

namespace dory::domain::events
{
    template<class... Ts>
    class Callable
    {
        public:
            virtual ~Callable() = default;
            
            virtual void operator()(Ts... arguments) const = 0;
    };

    template<class... Ts>
    class CallableFunction: public Callable<Ts...>
    {
        private:
            std::function<void(Ts...)> function;

        public:
            explicit CallableFunction(std::function<void(Ts...)>&& function):
                function(std::move(function))
            {}

            void operator()(Ts... arguments) const override
            {
                std::invoke(function, arguments...);
            }
    };

    template<typename T, class... Ts>
    class CallableMemberFunction: public Callable<Ts...>
    {
        private:
            std::pair<T*, void (T::*)(Ts...)> memberFunctionPair;

        public:
            CallableMemberFunction(T* instance, void (T::* memberFunction)(Ts...)):
                memberFunctionPair(instance, memberFunction)
            {}

            void operator()(Ts... arguments) const override
            {
                std::invoke(memberFunctionPair.second, memberFunctionPair.first, arguments...);
            }
    };

    template<class... Ts>
    class Event
    {
        public:
            using KeyType = std::size_t;

        private:
            KeyType idCounter {};

        protected:
            std::map<KeyType, std::shared_ptr<Callable<Ts...>>> handlers;

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
            }
    };

    template<class... Ts>
    class EventDispatcher: public Event<Ts...>
    {
        public:
            void operator()(Ts... arguments)
            {
                for (const auto& [key, handler]: this->handlers)
                {
                    handler->operator()(arguments...);
                }
            }
    };
}