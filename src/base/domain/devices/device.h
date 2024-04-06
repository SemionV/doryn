#pragma once

#include "base/typeComponents.h"

namespace dory::domain::devices
{
    template<typename TImpelementation, typename TDataContext>
    struct IDevice
    {
        void connect(TDataContext& context)
        {
            toImplementation<TImpelementation>(this)->connectImpl(context);
        }

        void disconnect(TDataContext& context)
        {
            toImplementation<TImpelementation>(this)->disconnectImpl(context);
        }
    };

    template<typename TImpelementation, typename TOutputData>
    struct IStandartOutputDevice
    {
        void out(const TOutputData& data)
        {
            toImplementation<TImpelementation>(this)->outImpl(data);
        }

        void flush()
        {
            toImplementation<TImpelementation>(this)->flushImpl();
        }
    };
}