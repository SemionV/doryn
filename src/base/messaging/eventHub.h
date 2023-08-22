#pragma once

#include "event.h"

namespace dory
{
    class EventsHubDispatcher
    {
        public:
            virtual void submit() = 0;
    };
    
    template<typename TEventData>
    class EventHub
    {
        private:
            EventDispatcher<TEventData&> event;

        public:
            Event<TEventData&>& getEvent()
            {
                return event;
            }

        protected:
            EventDispatcher<TEventData&>& getDispatcher()
            {
                return event;
            }
    };

    template<typename TEventData>
    class EventHubDispatcher: public EventHub<TEventData>
    {
        private:
            std::vector<TEventData> eventCases;

        public:
            void addCase(TEventData&& eventData)
            {
                eventCases.emplace_back(std::forward<TEventData>(eventData));
            }

            void submitCases()
            {
                EventDispatcher<TEventData&>& eventDispatcher = this->getDispatcher();
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