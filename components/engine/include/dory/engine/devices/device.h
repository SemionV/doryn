#pragma once

#include "dory/generics/typeTraits.h"

namespace dory::engine::devices
{
    template<typename TImplementation, typename TDataContext>
    struct IDevice
    {
        void connect(TDataContext& context)
        {
            toImplementation<TImplementation>(this)->connectImpl(context);
        }

        void disconnect(TDataContext& context)
        {
            toImplementation<TImplementation>(this)->disconnectImpl(context);
        }
    };

    template<typename TImplementation, typename TOutputData>
    struct IStandardOutputDevice
    {
        void out(const TOutputData& data)
        {
            toImplementation<TImplementation>(this)->outImpl(data);
        }

        void flush()
        {
            toImplementation<TImplementation>(this)->flushImpl();
        }
    };
}