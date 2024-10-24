#pragma once

#include <dory/core/services/iLogService.h>
#include <dory/core/implementation.h>
#include <dory/core/registry.h>
#include <dory/core/resources/configuration.h>
#include <memory>
#include <ranges>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/null_sink.h>

namespace dory::core::services
{
    template<typename TMessage, typename TImplPolicy, typename TState>
    class LogServiceGeneric: public implementation::ImplementationLevel<TImplPolicy, TState>
    {
    public:
        inline void trace(const TMessage& message) override
        {
            if(this->_logger)
            {
                this->_logger->trace(message);
            }
        }

        inline void debug(const TMessage& message) override
        {
            if(this->_logger)
            {
                this->_logger->debug(message);
            }
        }

        inline void information(const TMessage& message) override
        {
            if(this->_logger)
            {
                this->_logger->info(message);
            }
        }

        inline void warning(const TMessage& message) override
        {
            if(this->_logger)
            {
                this->_logger->warn(message);
            }
        }

        inline void error(const TMessage& message) override
        {
            if(this->_logger)
            {
                this->_logger->error(message);
            }
        }

        inline void critical(const TMessage& message) override
        {
            if(this->_logger)
            {
                this->_logger->critical(message);
            }
        }
    };

    class TerminalSink: public spdlog::sinks::sink
    {
    private:
        std::unique_ptr<spdlog::sinks::sink> _sink;
        Registry& _registry;

    public:
        explicit TerminalSink(std::unique_ptr<spdlog::sinks::sink> sink, Registry& registry);
        ~TerminalSink() override = default;

        void log(const spdlog::details::log_msg &msg) final;
        void flush() final;
        void set_pattern(const std::string &pattern) final;
        void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) final;
    };

    class LogServiceRoot
    {
    protected:
        std::shared_ptr<spdlog::logger> _logger;
    };

    class MultiSinkLogService: public LogServiceRoot, public IMultiSinkLogService
    {
    private:
        void buildLogger(const std::string& loggerName, const std::ranges::range auto& sinks)
        {
            this->_logger = std::make_shared<spdlog::logger>(loggerName, sinks.begin(), sinks.end());
        }

        static auto createSink(const resources::configuration::RotationLogSink& sinkConfig)
        {
            return std::make_shared<spdlog::sinks::rotating_file_sink_mt>(sinkConfig.logFileName,
                                                                          sinkConfig.maximumFileSize,
                                                                          sinkConfig.maximumFilesCount);
        }

        static auto createSink(const resources::configuration::StdoutLogSink& sinkConfig, Registry& registry)
        {
            auto consoleSinkImpl = std::make_unique<spdlog::sinks::stdout_color_sink_mt>();
            return std::make_shared<TerminalSink>(std::move(consoleSinkImpl), registry);
        }

    public:
        void initialize(const resources::configuration::Logger& loggerConfiguration, Registry& registry) override
        {
            std::shared_ptr<spdlog::sinks::sink> consoleSink;
            if(loggerConfiguration.stdoutLogger)
            {
                auto consoleSinkImpl = std::make_unique<spdlog::sinks::stdout_color_sink_mt>();
                consoleSink = createSink(*loggerConfiguration.stdoutLogger, registry);
            }
            else
            {
                consoleSink = std::make_shared<spdlog::sinks::null_sink_mt>();
            }

            std::shared_ptr<spdlog::sinks::sink> rotationFileSink;
            if(loggerConfiguration.rotationLogger)
            {
                rotationFileSink = createSink(*loggerConfiguration.rotationLogger);
            }
            else
            {
                rotationFileSink = std::make_shared<spdlog::sinks::null_sink_mt>();
            }

            std::array<std::shared_ptr<spdlog::sinks::sink>, 2> sinks = { consoleSink, rotationFileSink };
            buildLogger(loggerConfiguration.name, sinks);
        }
    };

    template<typename TPolicy>
    class LoggerHierarchyTop: public MultiSinkLogService
    {};

    struct LogServicePolicy: implementation::ImplementationPolicy<implementation::ImplementationList<LogServiceGeneric>, LoggerHierarchyTop>
    {};

    using LogService = implementation::Implementation<generic::TypeList<>, ILogService::MessageTypes, LogServicePolicy>;
}
