#pragma once

#include <dory/events.h>

namespace dory::engine::events::mainController
{
    struct Initialize
    {
    };

    struct Stop
    {
    };

    using EventTypes = generics::TypeList<const Initialize, const Stop>;

    template<typename TDataContext>
    using Hub = dory::events::EventHub<TDataContext, EventTypes>;

    template<typename TDataContext>
    using Dispatcher = dory::events::EventCannon<Hub<TDataContext>>;
}