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

        template<typename T, typename... Ts>
        void trace(T messageTemplate, Ts&&... parameters)
        {
            this->toImplementation()->traceImpl(messageTemplate, std::forward<Ts>(parameters)...);
        }

        template<typename T>
        void debug(const T& message)
        {
            this->toImplementation()->debugImpl(message);
        }

        template<typename T, typename... Ts>
        void debug(T messageTemplate, Ts&&... parameters)
        {
            this->toImplementation()->debugImpl(messageTemplate, std::forward<Ts>(parameters)...);
        }

        template<typename T>
        void information(const T& message)
        {
            this->toImplementation()->informationImpl(message);
        }

        template<typename T, typename... Ts>
        void information(const T& messageTemplate, Ts&&... parameters)
        {
            this->toImplementation()->informationImpl(messageTemplate, std::forward<Ts>(parameters)...);
        }

        template<typename T>
        void warning(const T& message)
        {
            this->toImplementation()->warningImpl(message);
        }

        template<typename T, typename... Ts>
        void warning(T messageTemplate, Ts&&... parameters)
        {
            this->toImplementation()->warningImpl(messageTemplate, std::forward<Ts>(parameters)...);
        }

        template<typename T>
        void error(const T& message)
        {
            this->toImplementation()->errorImpl(message);
        }

        template<typename T, typename... Ts>
        void error(T messageTemplate, Ts&&... parameters)
        {
            this->toImplementation()->errorImpl(messageTemplate, std::forward<Ts>(parameters)...);
        }

        template<typename T>
        void critical(const T& message)
        {
            this->toImplementation()->criticalImpl(message);
        }

        template<typename T, typename... Ts>
        void critical(T messageTemplate, Ts&&... parameters)
        {
            this->toImplementation()->criticalImpl(messageTemplate, std::forward<Ts>(parameters)...);
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
        explicit RotationLogService(const std::string& loggerName, std::filesystem::path logsDirectory)
        {
            logger = spdlog::rotating_logger_mt(loggerName, logsDirectory.append(baseFileName), maxSize, maxFiles);
        }

        template<typename T>
        void traceImpl(const T& message)
        {
            logger->trace(message);
        }

        template<typename T, typename... Ts>
        void traceImpl(T messageTemplate, Ts&&... parameters)
        {
            logger->trace(messageTemplate, std::forward<Ts>(parameters)...);
        }

        template<typename T>
        void debugImpl(const T& message)
        {
            logger->debug(message);
        }

        template<typename T, typename... Ts>
        void debugImpl(T messageTemplate, Ts&&... parameters)
        {
            logger->debug(messageTemplate, std::forward<Ts>(parameters)...);
        }

        template<typename T>
        void informationImpl(const T& message)
        {
            logger->info(message);
        }

        template<typename T, typename... Ts>
        void informationImpl(const T& messageTemplate, Ts&&... parameters)
        {
            logger->info(messageTemplate, std::forward<Ts>(parameters)...);
        }

        template<typename T>
        void warningImpl(const T& message)
        {
            logger->warn(message);
        }

        template<typename T, typename... Ts>
        void warningImpl(T messageTemplate, Ts&&... parameters)
        {
            logger->warn(messageTemplate, std::forward<Ts>(parameters)...);
        }

        template<typename T>
        void errorImpl(const T& message)
        {
            logger->error(message);
        }

        template<typename T, typename... Ts>
        void errorImpl(T messageTemplate, Ts&&... parameters)
        {
            logger->error(messageTemplate, std::forward<Ts>(parameters)...);
        }

        template<typename T>
        void criticalImpl(const T& message)
        {
            logger->critical(message);
        }

        template<typename T, typename... Ts>
        void criticalImpl(T messageTemplate, Ts&&... parameters)
        {
            logger->critical(messageTemplate, std::forward<Ts>(parameters)...);
        }
    };
}