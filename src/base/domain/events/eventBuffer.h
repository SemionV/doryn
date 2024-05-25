#pragma once

#include "event.h"

namespace dory::domain::events
{
    template<typename TDataContext, typename TEventData>
    class EventBuffer
    {
        private:
            std::shared_ptr<std::vector<TEventData>> eventCases;
            std::shared_ptr<std::vector<TEventData>> eventCasesBackBuffer;
            std::mutex mutex;

        public:
            EventBuffer():
                eventCases(std::make_shared<std::vector<TEventData>>()),
                eventCasesBackBuffer(std::make_shared<std::vector<TEventData>>())
            {}

            void addCase(const TEventData& eventData)
            {
                std::lock_guard lock(mutex);
                eventCasesBackBuffer->emplace_back(std::move(eventData));
            }

            template<typename TDispatcher>
            void submitCases(TDispatcher& eventDispatcher, TDataContext& dataContext)
            {
                std::lock_guard lock{mutex};

                std::shared_ptr<std::vector<TEventData>> temp = eventCases;
                eventCases = eventCasesBackBuffer;
                eventCasesBackBuffer = temp;

                for(std::size_t i = 0; i < eventCases->size(); ++i)
                {
                    try
                    {
                        eventDispatcher(dataContext, eventCases->operator[](i));
                    }
                    catch(const std::exception& e)
                    {                
                    }
                }
                eventCases->clear();
            }
    };
}