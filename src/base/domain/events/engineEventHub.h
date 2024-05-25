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

    /*template<class TDataContext>
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
    };*/

    template<typename TDataContext, typename TEventData>
    class EventController
    {
    protected:
        EventDispatcher<TDataContext&, TEventData&> eventDispatcher;
    };

    template<typename TDataContext, typename... TEvents>
    class EventHub: Uncopyable, public EventController<TDataContext, TEvents>...
    {
    protected:
        template<typename TEvent>
        EventDispatcher<TDataContext&, TEvent&>& getDispatcher()
        {
            return EventController<TDataContext, TEvent>::eventDispatcher;
        }

    public:
        template<typename TEvent>
        Event<TDataContext&, TEvent&>& on()
        {
            return getDispatcher<TEvent>();
        }
    };

    template<typename TEventHub>
    class EventCannon;

    template<typename TDataContext, typename... TEvents>
    class EventCannon<EventHub<TDataContext, TEvents...>>: public EventHub<TDataContext, TEvents...>
    {
    public:
        template<typename TEvent>
        void fire(TDataContext& context, TEvent& eventData)
        {
            auto dispatcher = this->template getDispatcher<TEvent>();
            dispatcher(context, eventData);
        }
    };

    template<typename TEventHub>
    class EventCannonBuffer;

    template<typename TDataContext, typename TEventData>
    class EventBufferController
    {
    protected:
        EventBuffer<TDataContext, TEventData> eventBuffer;
    };

    template<class TDataContext, typename... TEvents>
    class EventCannonBuffer<EventHub<TDataContext, TEvents...>>: public EventBufferController<TDataContext, TEvents>...,
                                                                 public EventHub<TDataContext, TEvents...>
    {
    public:
        template<typename TEvent>
        void addCharge(TEvent& eventData)
        {
            EventBufferController<TDataContext, TEvent>::eventBuffer.addCase(eventData);
        }

        void fireAll(TDataContext& context)
        {
            (EventBufferController<TDataContext, TEvents>::eventBuffer.submitCases(this->template getDispatcher<TEvents>(), context), ...);
        }
    };

    template<typename TDataContext>
    using EngineEventHub = EventHub<TDataContext, const InitializeEngineEventData, const StopEngineEventData>;

    template<typename TDataContext>
    using EngineEventHubDispatcher = EventCannon<EngineEventHub<TDataContext>>;
}