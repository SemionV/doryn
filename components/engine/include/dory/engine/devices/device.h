#pragma once

#include "dory/generics/typeTraits.h"

namespace dory::engine::devices
{
    template<typename TImplementation, typename TDataContext>
    struct IDevice
    {
        void connect(TDataContext& context)
        {
            static_cast<TImplementation*>(this)->connectImpl(context);
        }

        void disconnect(TDataContext& context)
        {
            static_cast<TImplementation*>(this)->disconnectImpl(context);
        }
    };

    template<typename TImplementation, typename TOutputData>
    struct IStandardOutputDevice
    {
        void out(const TOutputData& data)
        {
            static_cast<TImplementation*>(this)->outImpl(data);
        }

        void flush()
        {
            static_cast<TImplementation*>(this)->flushImpl();
        }
    };
}