#pragma once

#include "base/dependencies.h"
#include "base/doryExport.h"

namespace dory::domain::events
{
    template<class... Ts>
    class Event
    {
        private:
            int idCounter;

        protected:
            std::map<int, std::function<void(Ts...)>> handlers;

        public:
            Event():
                idCounter(0)
            {                
            }

            template<typename F>
            int attachHandler(F&& function)
            {
                auto functor = std::forward<F>(function);
                return attachFunction(std::move(functor));
            }

            template<typename F>
            int operator+=(F&& function)
            {
                return attachHandler(std::forward<F>(function));
            }

            int attachHandler(std::function<void(Ts...)>&& functor)
            {
                return attachFunction(std::forward<std::function<void(Ts...)>>(functor));
            }

            template<typename F>
            int operator+=(std::function<void(Ts...)>&& functor)
            {
                return attachFunction(std::forward<std::function<void(Ts...)>>(functor));
            }

            void detachHandler(int handlerKey)
            {
                handlers.erase(handlerKey);
            }

        private:
            int attachFunction(std::function<void(Ts...)>&& functor)
            {
                int key = getNewKey();
                handlers.emplace(key, std::forward<std::function<void(Ts...)>>(functor));

                return key;
            }

            int getNewKey()
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
                    std::invoke(handler, arguments...);
                }
            }
    };
}