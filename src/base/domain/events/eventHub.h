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

            void addCase(TEventData&& eventData)
            {
                std::lock_guard<std::mutex> lock(mutex);
                eventCasesBackBuffer->emplace_back(std::forward<TEventData>(eventData));
            }

            void submitCases(EventDispatcher<TDataContext&, TEventData&>& eventDispatcher, TDataContext& dataContext)
            {
                std::unique_lock<std::mutex> lock(mutex);

                std::vector<TEventData>* temp = eventCases;
                eventCases = eventCasesBackBuffer;
                eventCasesBackBuffer = temp;

                lock.unlock();

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