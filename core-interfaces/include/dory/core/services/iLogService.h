#pragma once

#include <string>
#include <dory/generic/typeList.h>
#include <dory/core/resources/configuration.h>

namespace dory::core
{
    class Registry;

    namespace services
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

            using ILogServiceGeneric<TMessages>::trace...;
            using ILogServiceGeneric<TMessages>::debug...;
            using ILogServiceGeneric<TMessages>::information...;
            using ILogServiceGeneric<TMessages>::warning...;
            using ILogServiceGeneric<TMessages>::error...;
            using ILogServiceGeneric<TMessages>::critical...;

            virtual ~ILogServiceBundle() = default;
        };

        using ILogService = ILogServiceBundle<std::string_view, std::string>;

        class IMultiSinkLogService: public ILogService
        {
        public:
            virtual void initialize(const resources::configuration::Logger& loggerConfiguration, Registry& registry) = 0;
        };
    }
}
