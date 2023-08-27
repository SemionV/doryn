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
                eventCasesBackBuffer->emplace_back(std::forward<TEventData>(eventData));
            }

            void submitCases(EventDispatcher<DataContext&, TEventData&>& eventDispatcher, DataContext& dataContext)
            {
                {
                    const std::lock_guard<std::mutex> lock(mutex);

                    std::vector<TEventData>* temp = eventCases;             
                    eventCases = eventCasesBackBuffer;
                    eventCasesBackBuffer = temp;
                }

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