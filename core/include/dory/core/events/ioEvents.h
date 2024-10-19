#pragma once

#include <dory/core/events/eventTypes.h>
#include "dory/core/generic/events.h"

namespace dory::core::events::io
{
    class EventDispatcher: public IEventDispatcher, public IEventHub
    {
    private:
        using EventListType = generic::TypeList<KeyPressEvent>;
        using EventHubType = dory::core::events::EventHub<resources::DataContext, EventListType>;
        using EventDispatcherType = dory::core::events::EventCannonBuffer<EventHubType>;

        EventDispatcherType _dispatcher;
        EventHubType& _hub = _dispatcher;

    public:
        void charge(KeyPressEvent eventData) final;
        void fireAll(resources::DataContext& context) final;
        std::size_t attach(std::function<void(resources::DataContext&, KeyPressEvent&)> handler) final;
    };
}
