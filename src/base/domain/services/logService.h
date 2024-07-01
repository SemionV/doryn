#pragma once

#include "dependencies.h"
#include "typeComponents.h"
#include "domain/configuration.h"
#include "domain/devices/terminalDevice.h"

namespace dory::domain::services
{
    template<typename TImplementation>
    class ILogService: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename T>
        void trace(const T& message)
        {
            this->toImplementation()->traceImpl(message);
        }

        template<typename T>
        void debug(const T& message)
        {
            this->toImplementation()->debugImpl(message);
        }

        template<typename T>
        void information(const T& message)
        {
            this->toImplementation()->informationImpl(message);
        }

        template<typename T>
        void warning(const T& message)
        {
            this->toImplementation()->warningImpl(message);
        }

        template<typename T>
        void error(const T& message)
        {
            this->toImplementation()->errorImpl(message);
        }

        template<typename T>
        void critical(const T& message)
        {
            this->toImplementation()->criticalImpl(message);
        }
    };

    class LogServiceNull: public ILogService<LogServiceNull>
    {
    public:
        template<typename T>
        static void traceImpl(const T& message)
        {
        }

        template<typename T>
        static void debugImpl(const T& message)
        {
        }

        template<typename T>
        static void informationImpl(const T& message)
        {
        }

        template<typename T>
        static void warningImpl(const T& message)
        {
        }

        template<typename T>
        static void errorImpl(const T& message)
        {
        }

        template<typename T>
        static void criticalImpl(const T& message)
        {
        }
    };

    template<class TImplementation>
    class LogService: public ILogService<TImplementation>
    {
    protected:
        std::shared_ptr<spdlog::logger> logger;

    public:
        template<typename T>
        void traceImpl(const T& message)
        {
            if(logger)
            {
                logger->trace(message);
            }
        }

        template<typename T>
        void debugImpl(const T& message)
        {
            if(logger)
            {
                logger->debug(message);
            }
        }

        template<typename T>
        void informationImpl(const T& message)
        {
            if(logger)
            {
                logger->info(message);
            }
        }

        template<typename T>
        void warningImpl(const T& message)
        {
            if(logger)
            {
                logger->warn(message);
            }
        }

        template<typename T>
        void errorImpl(const T& message)
        {
            if(logger)
            {
                logger->error(message);
            }
        }

        template<typename T>
        void criticalImpl(const T& message)
        {
            if(logger)
            {
                logger->critical(message);
            }
        }
    };

    template<typename TTerminal>
    class TerminalSink: public spdlog::sinks::sink
    {
    private:
        std::unique_ptr<spdlog::sinks::sink> sink;

        using TerminalType = devices::ITerminal<TTerminal>;
        TerminalType& terminal;

    public:
        explicit TerminalSink(std::unique_ptr<spdlog::sinks::sink> sink, TerminalType& terminal):
            sink(std::move(sink)), terminal(terminal)
        {}

        ~TerminalSink() override = default;

        void log(const spdlog::details::log_msg &msg) override
        {
            auto isCommandMode = terminal.isCommandMode();
            if(isCommandMode)
            {
                terminal.exitCommandMode();
            }

            try
            {
                sink->log(msg);
            }
            catch(...)
            {}

            if(isCommandMode)
            {
                terminal.enterCommandMode();
            }
        }

        void flush() override
        {
            sink->flush();
        }

        void set_pattern(const std::string &pattern) override
        {
            sink->set_pattern(pattern);
        }

        void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override
        {
            sink->set_formatter(std::move(sink_formatter));
        }
    };

    class MultiSinkLogService: public LogService<MultiSinkLogService>
    {
    public:
        template<typename TTerminal>
        void initialize(const configuration::Logger& loggerConfiguration, OptionalReference<TTerminal> terminal)
        {
            std::shared_ptr<spdlog::sinks::sink> consoleSink;
            if(loggerConfiguration.stdoutLogger && terminal)
            {
                auto consoleSinkImpl = std::make_unique<spdlog::sinks::stdout_color_sink_mt>();
                consoleSink = std::make_shared<TerminalSink<TTerminal>>(std::move(consoleSinkImpl), *terminal);
            }
            else
            {
                consoleSink = std::make_shared<spdlog::sinks::null_sink_mt>();
            }

            std::shared_ptr<spdlog::sinks::sink> rotationFileSink;
            if(loggerConfiguration.rotationLogger)
            {
                rotationFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(loggerConfiguration.rotationLogger->logFileName,
                                                                                          loggerConfiguration.rotationLogger->maximumFileSize,
                                                                                          loggerConfiguration.rotationLogger->maximumFilesCount);
            }
            else
            {
                rotationFileSink = std::make_shared<spdlog::sinks::null_sink_mt>();
            }

            this->logger = std::make_shared<spdlog::logger>(loggerConfiguration.name, spdlog::sinks_init_list{rotationFileSink, consoleSink});
        }
    };

    class StdOutLogService: public LogService<StdOutLogService>
    {
    public:
        explicit StdOutLogService(const std::string& loggerName)
        {
            this->logger = std::make_shared<spdlog::logger>(loggerName, spdlog::sinks_init_list{std::make_unique<spdlog::sinks::stdout_color_sink_mt>()});
        }
    };
}