#pragma once

#include "base/dependencies.h"
#include "base/messaging/event.h"
#include "base/messaging/eventHub.h"

namespace dory::events
{
    struct InitializeEngineEventData
    {
    };

    class EngineEventHub
    {
        private:
            EventDispatcher<DataContext&, const InitializeEngineEventData&> initializeEngineEvent;

        protected:
            EventDispatcher<DataContext&, const InitializeEngineEventData&>& onInitializeEngineDispatcher()
            {
                return initializeEngineEvent;
            }

        public:
            Event<DataContext&, const InitializeEngineEventData&>& onInitializeEngine()
            {
                return initializeEngineEvent;
            }
    };

    class EngineEventHubDispatcher: public EngineEventHub
    {
        public:
            void fire(DataContext& context, const InitializeEngineEventData& initializeEngineEventData)
            {
                auto dispatcher = onInitializeEngineDispatcher();
                dispatcher(context, initializeEngineEventData);
            }
    };
}