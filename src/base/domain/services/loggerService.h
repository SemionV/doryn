#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"

namespace dory::domain::services
{
    template<typename TImplementation>
    class ILoggerService: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        void debug(const std::string message)
        {
            this->toImplementation()->debugImpl(message);
        }

        void information(const std::string message)
        {
            this->toImplementation()->infoImpl(message);
        }

        void warning(const std::string message)
        {
            this->toImplementation()->warningImpl(message);
        }

        void error(const std::string message)
        {
            this->toImplementation()->errorImpl(message);
        }

        void critical(const std::string message)
        {
            this->toImplementation()->criticalImpl(message);
        }
    };
}