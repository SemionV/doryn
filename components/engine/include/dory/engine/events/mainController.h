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

    template<typename TDataContext>
    using Hub = dory::events::EventHub<TDataContext, const Initialize, const Stop>;

    template<typename TDataContext>
    using Dispatcher = dory::events::EventCannon<Hub<TDataContext>>;
}