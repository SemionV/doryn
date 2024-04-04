#pragma once

#include "event.h"

namespace dory::domain::events
{
    template<class TDataContext>
    class EventHubDispatcher
    {
        public:
            virtual ~EventHubDispatcher() = default;

            virtual void submit(TDataContext& context) = 0;
    };

    template<typename TDataContext, typename TEventData>
    class EventBuffer
    {
        private:
            std::vector<TEventData>* eventCases;
            std::vector<TEventData>* eventCasesBackBuffer;
            std::mutex mutex;

        public:
            EventBuffer():
                eventCases(new std::vector<TEventData>()),
                eventCasesBackBuffer(new std::vector<TEventData>())
            {                
            }

            virtual ~EventBuffer()
            {       
                delete eventCases;     
                delete eventCasesBackBuffer;     
            }

            void addCase(const TEventData& eventData)
            {
                std::lock_guard lock(mutex);
                eventCasesBackBuffer->emplace_back(std::move(eventData));
            }

            void submitCases(EventDispatcher<TDataContext&, TEventData&>& eventDispatcher, TDataContext& dataContext)
            {
                std::lock_guard lock{mutex};

                std::vector<TEventData>* temp = eventCases;
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