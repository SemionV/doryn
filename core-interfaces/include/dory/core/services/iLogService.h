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

    //using ILogService = ILogServiceBundle<std::string_view, std::string>;
    class ILogService
    {
    public:
        using MessageTypes = generic::TypeList<std::string_view, std::string>;

        virtual ~ILogService() = default;

        virtual void trace(const std::string_view& message) = 0;
        virtual void trace(const std::string& message) = 0;
        virtual void debug(const std::string_view& message) = 0;
        virtual void debug(const std::string& message) = 0;
        virtual void information(const std::string_view& message) = 0;
        virtual void information(const std::string& message) = 0;
        virtual void warning(const std::string_view& message) = 0;
        virtual void warning(const std::string& message) = 0;
        virtual void error(const std::string_view& message) = 0;
        virtual void error(const std::string& message) = 0;
        virtual void critical(const std::string_view& message) = 0;
        virtual void critical(const std::string& message) = 0;
    };
}
