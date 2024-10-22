#pragma once

#include <dory/core/services/iLogService.h>
#include <dory/core/generic/implementation.h>

#include <iostream>

namespace dory::core::services
{
    template<typename TMessage, typename TImplPolicy, typename TState>
    class LogServiceNullGeneric: public generic::implementation::ImplementationLevel<TImplPolicy, TState>
    {
    public:
        inline void trace(const TMessage& message) final
        {}

        inline void debug(const TMessage& message) final
        {}

        inline void information(const TMessage& message) final
        {}

        inline void warning(const TMessage& message) final
        {}

        inline void error(const TMessage& message) final
        {}

        inline void critical(const TMessage& message) final
        {}
    };

    struct LogServiceNullPolicy: generic::implementation::ImplementationPolicy<generic::implementation::ImplementationList<LogServiceNullGeneric>>
    {};

    using LogServiceNull = generic::implementation::Implementation<generic::TypeList<ILogService>, ILogService::MessageTypes, LogServiceNullPolicy>;
}
