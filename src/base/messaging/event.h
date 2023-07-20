#pragma once

#include "base/dependencies.h"

namespace dory
{
    template<class... Ts>
    class DORY_API Event
    {
        private:
            std::vector<std::function<void (Ts...)>> eventHandlers;

        public:
            template <typename F>
            void operator+=(F&& function)
            {
                eventHandlers.push_back(std::forward<F>(function));               
            }

            void operator()(Ts... arguments)
            {
                std::size_t size = eventHandlers.size();
                for(std::size_t i = 0; i < size; i++)
                {
                    auto handler = eventHandlers[i];
                    std::invoke(handler, arguments...);
                }         
            }
    };
}