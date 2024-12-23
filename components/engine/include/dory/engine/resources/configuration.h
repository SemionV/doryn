#pragma once

#include <map>
#include <string>
#include <vector>
#include <refl.hpp>

namespace dory::engine::resources::configuration
{
    struct RecursiveSection
    {
        std::vector<std::string> loadFrom;
        std::string saveTo;
        std::string description;
    };
}
REFL_TYPE(dory::engine::resources::configuration::RecursiveSection)
    REFL_FIELD(loadFrom)
    REFL_FIELD(saveTo)
    REFL_FIELD(description)
REFL_END

namespace dory::engine::resources::configuration {
    struct RotationLogSink
    {
        std::string logFileName = "logs/main.log";
        std::size_t maximumFileSize = 1048576 * 5;
        std::size_t maximumFilesCount = 3;
    };
}
REFL_TYPE(dory::engine::resources::configuration::RotationLogSink)
    REFL_FIELD(logFileName)
    REFL_FIELD(maximumFileSize)
    REFL_FIELD(maximumFilesCount)
REFL_END

namespace dory::engine::resources::configuration {
    struct StdoutLogSink
    {
    };
}
REFL_TYPE(dory::engine::resources::configuration::StdoutLogSink)
REFL_END

namespace dory::engine::resources::configuration {
    struct Logger
    {
        std::string name;
        std::optional<RotationLogSink> rotationLogger;
        std::optional<StdoutLogSink> stdoutLogger;
    };
}
REFL_TYPE(dory::engine::resources::configuration::Logger)
    REFL_FIELD(name)
    REFL_FIELD(rotationLogger)
    REFL_FIELD(stdoutLogger)
REFL_END


namespace dory::engine::resources::configuration {
    struct LoggingConfiguration
    {
        Logger mainLogger;
        Logger configurationLogger;
    };
}
REFL_TYPE(dory::engine::resources::configuration::LoggingConfiguration)
    REFL_FIELD(mainLogger)
    REFL_FIELD(configurationLogger)
REFL_END

namespace dory::engine::resources::configuration {
    struct ShaderLoader
    {
        std::string shadersDirectory;
    };
}
REFL_TYPE(dory::engine::resources::configuration::ShaderLoader)
    REFL_FIELD(shadersDirectory)
REFL_END

namespace dory::engine::resources::configuration {
    struct Interface
    {
        RecursiveSection section;
        std::string activeLanguage;
    };
}
REFL_TYPE(dory::engine::resources::configuration::Interface)
    REFL_FIELD(section)
    REFL_FIELD(activeLanguage)
REFL_END

namespace dory::engine::resources::configuration {
    struct BuildInfo
    {
        std::string commitSHA;
        std::string timestamp;
        std::string version;
    };
}
REFL_TYPE(dory::engine::resources::configuration::BuildInfo)
        REFL_FIELD(commitSHA)
        REFL_FIELD(timestamp)
        REFL_FIELD(version)
REFL_END

namespace dory::engine::resources::configuration {
    struct Configuration
    {
        RecursiveSection section;
        BuildInfo buildInfo;
        std::map<std::string, std::string> modules;
        std::map<std::string, std::vector<std::string>> localizations;
        LoggingConfiguration loggingConfiguration;
        ShaderLoader shaderLoader;
        Interface userInterface;
    };
}
REFL_TYPE(dory::engine::resources::configuration::Configuration)
    REFL_FIELD(section)
    REFL_FIELD(buildInfo)
    REFL_FIELD(localizations)
    REFL_FIELD(loggingConfiguration)
    REFL_FIELD(shaderLoader)
    REFL_FIELD(userInterface)
REFL_END