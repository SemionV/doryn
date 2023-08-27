#pragma once

#include "event.h"

namespace dory
{
    class EventHubDispatcher
    {
        public:
            virtual void submit(DataContext& context) = 0;
    };

    template<typename TEventData>
    class EventBuffer
    {
        private:
            std::vector<TEventData> eventCases;
            std::mutex mutex;

        public:
            void addCase(TEventData&& eventData)
            {
                const std::lock_guard<std::mutex> lock(mutex);

                eventCases.emplace_back(std::forward<TEventData>(eventData));
            }

            void submitCases(EventDispatcher<DataContext&, TEventData&>& eventDispatcher, DataContext& dataContext)
            {
                for(std::size_t i = 0; i < eventCases.size(); ++i)
                {
                    try
                    {
                        eventDispatcher(dataContext, eventCases[i]);
                    }
                    catch(const std::exception& e)
                    {                
                    }
                }
                eventCases.clear();
            }
    };
}