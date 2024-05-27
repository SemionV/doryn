#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"

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

    class RotationLogService: public ILogService<RotationLogService>
    {
    private:
        const std::size_t maxSize = 1048576 * 5;
        const std::size_t maxFiles = 3;
        const std::string baseFileName = "log.txt";
        std::shared_ptr<spdlog::logger> logger;

    public:
        explicit RotationLogService(const std::string& loggerName, const std::filesystem::path& logsDirectory)
        {
            auto path = std::filesystem::path{logsDirectory};
            logger = spdlog::rotating_logger_mt(loggerName, path.append(baseFileName).string(), maxSize, maxFiles);
        }

        template<typename T>
        void traceImpl(const T& message)
        {
            logger->trace(message);
        }

        template<typename T>
        void debugImpl(const T& message)
        {
            logger->debug(message);
        }

        template<typename T>
        void informationImpl(const T& message)
        {
            logger->info(message);
        }

        template<typename T>
        void warningImpl(const T& message)
        {
            logger->warn(message);
        }

        template<typename T>
        void errorImpl(const T& message)
        {
            logger->error(message);
        }

        template<typename T>
        void criticalImpl(const T& message)
        {
            logger->critical(message);
        }
    };
}