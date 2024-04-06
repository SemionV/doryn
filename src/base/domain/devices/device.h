#pragma once

#include "base/typeComponents.h"

namespace dory::domain::devices
{
    template<typename TImplementation, typename TDataContext>
    struct IDevice: Uncopyable, public StaticInterface<TImplementation>
    {
        void connect(TDataContext& context)
        {
            this->toImplementation()->connectImpl(context);
        }

        void disconnect(TDataContext& context)
        {
            this->toImplementation()->disconnectImpl(context);
        }
    };
}