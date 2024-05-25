#pragma once

#include "event.h"

namespace dory::domain::events
{
    struct InitializeEngineEventData
    {
    };

    struct StopEngineEventData
    {
    };

    template<typename TDataContext>
    using EngineEventHub = EventHub<TDataContext, const InitializeEngineEventData, const StopEngineEventData>;

    template<typename TDataContext>
    using EngineEventHubDispatcher = EventCannon<EngineEventHub<TDataContext>>;
}