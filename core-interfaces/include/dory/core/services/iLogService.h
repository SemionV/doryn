#pragma once

#include <string>
#include <dory/core/generic/typeList.h>

namespace dory::core::services
{
    template<typename TMessage>
    class ILogServiceGeneric
    {
    public:
        virtual void trace(const TMessage& message) = 0;
        virtual void debug(const TMessage& message) = 0;
        virtual void information(const TMessage& message) = 0;
        virtual void warning(const TMessage& message) = 0;
        virtual void error(const TMessage& message) = 0;
        virtual void critical(const TMessage& message) = 0;
    };

    template<typename... TMessages>
    class ILogServiceBundle: public ILogServiceGeneric<TMessages>...
    {
    public:
        using MessageTypes = generic::TypeList<TMessages...>;

        virtual ~ILogServiceBundle() = default;
    };

    template<typename... TMessages>
    class ILogServiceBundle<generic::TypeList<TMessages...>>: public ILogServiceBundle<TMessages...>
    {};

    using ILogService = ILogServiceBundle<std::string_view, std::string>;
}
