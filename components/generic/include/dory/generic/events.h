#pragma once

#include <functional>
#include "typeList.h"

namespace dory::generic::events
{
    template<typename TEvent>
    class IEventListener
    {
    public:
        virtual std::size_t attach(std::function<void(resources::DataContext&, TEvent&)> handler) = 0;
        virtual void detach(std::size_t eventKey, const TEvent& event) = 0;
    };

    template<typename... TEvents>
    class IEventsListener: public IEventListener<TEvents>...
    {
    public:
        virtual ~IEventsListener() = default;

        using IEventListener<TEvents>::attach...;
        using IEventListener<TEvents>::detach...;
    };

    template<typename... TEvents>
    class IEventsListener<generic::TypeList<TEvents...>>: public IEventsListener<TEvents...>
    {};

    template<typename TEvent>
    class IEventDispatcher
    {
    public:
        virtual void fire(resources::DataContext& context, TEvent& eventData) = 0;
    };

    template<typename... TEvents>
    class IEventsDispatcher: public IEventDispatcher<TEvents>...
    {
    public:
        virtual ~IEventsDispatcher() = default;

        using IEventDispatcher<TEvents>::fire...;
    };

    template<typename... TEvents>
    class IEventsDispatcher<generic::TypeList<TEvents...>>: public IEventsDispatcher<TEvents...>
    {};

    template<typename TEvent>
    class IEventBufferDispatcher
    {
    public:
        virtual void charge(TEvent eventData) = 0;
        virtual void fireAll(resources::DataContext& context) = 0;
    };

    template<typename... TEvents>
    class IEventsBufferDispatcher: public IEventBufferDispatcher<TEvents>...
    {
    public:
        virtual ~IEventsBufferDispatcher() = default;

        using IEventBufferDispatcher<TEvents>::charge...;
        using IEventBufferDispatcher<TEvents>::fireAll...;
    };

    template<typename... TEvents>
    class IEventsBufferDispatcher<generic::TypeList<TEvents...>>: public IEventsBufferDispatcher<TEvents...>
    {};

    template<typename... TEvents>
    struct EventBundle
    {
        using EventListType = generic::TypeList<TEvents...>;
        using IDispatcher = IEventsDispatcher<EventListType>;
        using IListener = IEventsListener<EventListType>;
    };

    template<typename... TEvents>
    struct EventBufferBundle
    {
        using EventListType = generic::TypeList<TEvents...>;
        using IDispatcher = IEventsBufferDispatcher<EventListType>;
        using IListener = IEventsListener<EventListType>;
    };
}
