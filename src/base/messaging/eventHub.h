#pragma once

#include "event.h"

namespace dory
{
    class EventHub
    {
        public:
            virtual void submit() = 0;
    };

    template<typename TEventData>
    class EventBuffer
    {
        private:
            std::vector<TEventData> eventCases;

        public:
            void addCase(TEventData&& eventData)
            {
                eventCases.emplace_back(std::forward<TEventData>(eventData));
            }

            void submitCases(EventDispatcher<TEventData&>& eventDispatcher)
            {
                for(std::size_t i = 0; i < eventCases.size(); ++i)
                {
                    try
                    {
                        eventDispatcher(eventCases[i]);
                    }
                    catch(const std::exception& e)
                    {                
                    }
                }
                eventCases.clear();
            }
    };
}