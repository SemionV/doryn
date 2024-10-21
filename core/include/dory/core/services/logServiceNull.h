#pragma once

#include <dory/core/services/iLogService.h>
#include <dory/core/generic/implementation.h>

#include <iostream>

namespace dory::core::services
{
    template<typename TInterface, typename TMessages>
    class LogServiceBundle;

    template<typename TMessage, typename TInterface, typename TMessages>
    class LogServiceNullGeneric: public LogServiceBundle<TInterface, TMessages>
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

    template<typename TInterface, typename TMessage, typename... TMessages>
    class LogServiceBundle<TInterface, generic::TypeList<TMessage, TMessages...>>: public LogServiceNullGeneric<TMessage, TInterface, generic::TypeList<TMessages...>>
    {};

    template<typename TInterface>
    class LogServiceBundle<TInterface, generic::TypeList<>>: public TInterface
    {};

    class LogServiceNull: public LogServiceBundle<ILogService, ILogService::MessageTypes>
    {};

    template<typename TMessage, typename TImplPolicy, typename TState>
    class LogServiceNullGeneric2: public generic::implementation::ImplementationLevel<TImplPolicy, TState>
    {
    public:
        inline void trace(const TMessage& message) final
        {
            std::cout << message << std::endl;
        }

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

    using LogServiceNull2 = generic::implementation::Implementation<generic::implementation::ImplementationPolicy<generic::TypeList<ILogService>, generic::implementation::ImplementationList<LogServiceNullGeneric2>, ILogService::MessageTypes>>;
}
