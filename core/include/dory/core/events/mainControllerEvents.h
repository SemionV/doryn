#pragma once

#include <dory/core/events/eventTypes.h>
#include "events.h"

namespace dory::core::events::mainController
{
    class EventDispatcher: public IEventDispatcher, public IEventHub
    {
    private:
        using EventListType = generic::TypeList<const Initialize, const Stop>;
        using EventHubType = dory::core::events::EventHub<resources::DataContext, EventListType>;
        using EventDispatcherType = dory::core::events::EventCannon<EventHubType>;

        EventDispatcherType _dispatcher;
        EventHubType& _hub = _dispatcher;

    public:
        void fire(resources::DataContext& context, const Initialize& eventData) final;
        void fire(resources::DataContext& context, const Stop& eventData) final;
        std::size_t attach(std::function<void(resources::DataContext&, const Initialize&)> handler) final;
        std::size_t attach(std::function<void(resources::DataContext&, const Stop&)> handler) final;
    };
}
