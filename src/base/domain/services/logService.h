#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/domain/configuration.h"

namespace dory::domain::services
{
    template<typename TImplementation>
    class ILogService: Uncopyable, public StaticInterface<TImplementation>
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

    class RotationLogService: public LogService<RotationLogService>
    {
    public:
        explicit RotationLogService(const std::string& loggerName,
                                    const std::filesystem::path& logsDirectory,
                                    const std::string& logFileName = "main.log",
                                    const std::size_t maximumFileSize = 1048576 * 5,//5Mb
                                    const std::size_t maximumFilesCount = 3)
        {
            auto path = std::filesystem::path{logsDirectory};
            this->logger = spdlog::rotating_logger_mt(loggerName, path.append(logFileName).string(), maximumFileSize, maximumFilesCount);
        }
    };

    class MultiSinkLogService: public LogService<MultiSinkLogService>
    {
    public:
        void initialize(const configuration::Logger& loggerConfiguration)
        {
            std::shared_ptr<spdlog::sinks::sink> consoleSink;
            if(loggerConfiguration.stdoutLogger)
            {
                consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
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
                consoleSink = std::make_shared<spdlog::sinks::null_sink_mt>();
            }

            this->logger = std::make_shared<spdlog::logger>(loggerConfiguration.name, spdlog::sinks_init_list{rotationFileSink, consoleSink});
        }
    };
}