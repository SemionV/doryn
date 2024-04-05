#pragma once

#include "base/dependencies.h"
#include "event.h"
#include "eventBuffer.h"
#include "base/typeComponents.h"

namespace dory::domain::events
{
    struct InitializeEngineEventData
    {
    };

    struct StopEngineEventData
    {
    };

    template<class TDataContext>
    class EngineEventHub: Uncopyable
    {
        private:
            EventDispatcher<TDataContext&, const InitializeEngineEventData&> initializeEngineEvent;
            EventDispatcher<TDataContext&, const StopEngineEventData&> stopEngineEvent;

        protected:
            EventDispatcher<TDataContext&, const InitializeEngineEventData&>& onInitializeEngineDispatcher()
            {
                return initializeEngineEvent;
            }

            EventDispatcher<TDataContext&, const StopEngineEventData&>& onStopEngineDispatcher()
            {
                return stopEngineEvent;
            }

        public:
            Event<TDataContext&, const InitializeEngineEventData&>& onInitializeEngine()
            {
                return initializeEngineEvent;
            }

            Event<TDataContext&, const StopEngineEventData&>& onStopEngine()
            {
                return stopEngineEvent;
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

            void fire(TDataContext& context, const StopEngineEventData& stopEngineEventData)
            {
                auto dispatcher = this->onStopEngineDispatcher();
                dispatcher(context, stopEngineEventData);
            }
    };
}