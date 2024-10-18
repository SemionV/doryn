#pragma once

#include <dory/core/events/eventTypes.h>
#include <dory/core/generic/events.h>

namespace dory::core::events::io
{
    class EventDispatcher: public IEventDispatcher, public IEventHub
    {
    private:
        using EventListType = generic::TypeList<const KeyCode>;
        using EventHubType = dory::core::events::EventHub<resources::DataContext, EventListType>;
        using EventDispatcherType = dory::core::events::EventCannon<EventHubType>;

        EventDispatcherType _dispatcher;
        EventHubType& _hub = _dispatcher;

    public:
        void fire(resources::DataContext& context, const KeyCode& eventData) final;
        std::size_t attach(std::function<void(resources::DataContext&, const KeyCode&)> handler) final;
    };
}
