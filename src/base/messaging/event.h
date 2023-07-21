#pragma once

#include "base/dependencies.h"

namespace dory
{
    template<class... Ts>
    class DORY_API Event
    {
        private:
            std::map<int, std::function<void(Ts...)>> handlers;
            int idCounter;

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

            int attachHandler(std::function<void(Ts...)>&& functor)
            {
                return attachFunction(std::forward<std::function<void(Ts...)>>(functor));
            }

            void operator()(Ts... arguments)
            {
                for (const auto& [key, handler]: handlers)
                {
                    std::invoke(handler, arguments...);
                }
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
}