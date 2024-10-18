#pragma once

#include <dory/core/events/eventTypes.h>
#include "events.h"

namespace dory::core::events::mainController
{
    class EventDispatcher: public EventDispatcherImpl<IDispatcher, EventListType>,
            public EventListenerImpl<IListener, EventListType>
    {};
}
