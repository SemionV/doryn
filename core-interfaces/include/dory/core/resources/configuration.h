#pragma once

#include <vector>
#include <string>
#include <map>
#include <optional>

namespace dory::core::resources::configuration
{
    struct RecursiveSection
    {
        std::vector<std::string> loadFrom;
        std::string saveTo;
        std::string description;
    };

    struct RotationLogSink
    {
        std::string logFileName = "logs/main.log";
        std::size_t maximumFileSize = 1048576 * 5;
        std::size_t maximumFilesCount = 3;
    };

    struct StdoutLogSink
    {
    };

    struct Logger
    {
        std::string name;
        std::optional<RotationLogSink> rotationLogger;
        std::optional<StdoutLogSink> stdoutLogger;
    };

    struct LoggingConfiguration
    {
        Logger mainLogger;
        Logger configurationLogger;
    };

    struct ShaderLoader
    {
        std::string shadersDirectory;
    };

    struct Interface
    {
        RecursiveSection section;
        std::string activeLanguage;
    };

    struct BuildInfo
    {
        std::string commitSHA;
        std::string timestamp;
        std::string version;
    };

    struct FilesSystemWatch
    {
        std::string directory;
        bool recursive;
    };

    struct Extension
    {
        std::string name;
        std::string path;
        bool reloadOnModification;
        std::vector<std::string> modules;
    };

    /*struct ImageStream
    {
        std::string directory;
    };

    struct Profiling
    {
        ImageStream
    };*/

    struct Configuration
    {
        RecursiveSection section;
        BuildInfo buildInfo;
        std::map<std::string, std::vector<std::string>> localizations;
        LoggingConfiguration loggingConfiguration;
        ShaderLoader shaderLoader;
        Interface userInterface;
        std::vector<FilesSystemWatch> fileSystemWatches;
        std::vector<Extension> extensions;
        std::map<std::string, std::string> assetTypes;
    };
}