#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"

namespace dory::domain::services
{
    template<typename TImplementation>
    class ILogService: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        void trace(const std::string& message)
        {
            this->toImplementation()->traceImpl(message);
        }

        void debug(const std::string& message)
        {
            this->toImplementation()->debugImpl(message);
        }

        void information(const std::string& message)
        {
            this->toImplementation()->informationImpl(message);
        }

        void warning(const std::string& message)
        {
            this->toImplementation()->warningImpl(message);
        }

        void error(const std::string& message)
        {
            this->toImplementation()->errorImpl(message);
        }

        void critical(const std::string& message)
        {
            this->toImplementation()->criticalImpl(message);
        }
    };

    class RotationLogService: public ILogService<RotationLogService>
    {
    public:
        const std::size_t maxSize = 1048576 * 5;
        const std::size_t maxFiles = 3;
        const std::string baseFileName = "log.txt";
        std::shared_ptr<spdlog::logger> logger;

        explicit RotationLogService(const std::string& loggerName, std::filesystem::path logsDirectory)
        {
            logger = spdlog::rotating_logger_mt(loggerName, logsDirectory.append(baseFileName), maxSize, maxFiles);
        }

        void traceImpl(const std::string& message)
        {
            logger->trace(message);
        }

        void debugImpl(const std::string& message)
        {
            logger->debug(message);
        }

        void informationImpl(const std::string& message)
        {
            logger->info(message);
        }

        void warningImpl(const std::string& message)
        {
            logger->warn(message);
        }

        void errorImpl(const std::string& message)
        {
            logger->error(message);
        }

        void criticalImpl(const std::string& message)
        {
            logger->critical(message);
        }
    };
}