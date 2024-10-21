#pragma once

#include <dory/core/services/iLogService.h>

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
}
