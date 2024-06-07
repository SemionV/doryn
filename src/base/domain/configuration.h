#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"

namespace dory::configuration
{
    struct RecursiveSection
    {
        std::vector<std::string> loadFrom;
        std::string saveTo;
        std::string description;
    };
}
REFL_TYPE(dory::configuration::RecursiveSection)
        REFL_FIELD(loadFrom)
        REFL_FIELD(saveTo)
        REFL_FIELD(description)
REFL_END

namespace dory::configuration {
    struct RotationLogSink
    {
        std::string logFileName = "logs/main.log";
        std::size_t maximumFileSize = 1048576 * 5;
        std::size_t maximumFilesCount = 3;
    };
}
REFL_TYPE(dory::configuration::RotationLogSink)
        REFL_FIELD(logFileName)
        REFL_FIELD(maximumFileSize)
        REFL_FIELD(maximumFilesCount)
REFL_END

namespace dory::configuration {
    struct StdoutLogSink
    {
    };
}
REFL_TYPE(dory::configuration::StdoutLogSink)
REFL_END

namespace dory::configuration {
    struct Logger
    {
        std::string name;
        std::optional<RotationLogSink> rotationLogger;
        std::optional<StdoutLogSink> stdoutLogger;
    };
}
REFL_TYPE(dory::configuration::Logger)
        REFL_FIELD(name)
        REFL_FIELD(rotationLogger)
        REFL_FIELD(stdoutLogger)
REFL_END


namespace dory::configuration {
    struct LoggingConfiguration
    {
        Logger mainLogger;
        Logger configurationLogger;
    };
}
REFL_TYPE(dory::configuration::LoggingConfiguration)
        REFL_FIELD(mainLogger)
        REFL_FIELD(configurationLogger)
REFL_END

namespace dory::configuration {
    struct ShaderLoader
    {
        std::string shadersDirectory;
    };
}
REFL_TYPE(dory::configuration::ShaderLoader)
        REFL_FIELD(shadersDirectory)
REFL_END

namespace dory::configuration {
    struct Interface
    {
        RecursiveSection section;
        std::string activeLanguage;
    };
}
REFL_TYPE(dory::configuration::Interface)
        REFL_FIELD(section)
        REFL_FIELD(activeLanguage)
REFL_END

namespace dory::configuration {
    struct Configuration
    {
        RecursiveSection section;
        std::map<std::string, std::vector<std::string>> localizations;
        LoggingConfiguration loggingConfiguration;
        ShaderLoader shaderLoader;
        Interface interface;
    };
}
REFL_TYPE(dory::configuration::Configuration)
        REFL_FIELD(section)
        REFL_FIELD(localizations)
        REFL_FIELD(loggingConfiguration)
        REFL_FIELD(shaderLoader)
        REFL_FIELD(interface)
REFL_END