#pragma once

#include "base/dependencies.h"
#include "event.h"
#include "eventHub.h"

namespace dory::domain::events
{
    struct InitializeEngineEventData
    {
    };

    template<class TDataContext>
    class EngineEventHub
    {
        private:
            EventDispatcher<TDataContext&, const InitializeEngineEventData&> initializeEngineEvent;

        protected:
            EventDispatcher<TDataContext&, const InitializeEngineEventData&>& onInitializeEngineDispatcher()
            {
                return initializeEngineEvent;
            }

        public:
            Event<TDataContext&, const InitializeEngineEventData&>& onInitializeEngine()
            {
                return initializeEngineEvent;
            }
    };

    template<class TDataContext>
    class EngineEventHubDispatcher: public EngineEventHub<TDataContext>
    {
        public:
            void fire(TDataContext& context, const InitializeEngineEventData& initializeEngineEventData)
            {
                auto dispatcher = this->onInitializeEngineDispatcher();
                dispatcher(context, initializeEngineEventData);
            }
    };
}