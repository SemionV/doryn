#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"

namespace dory::configuration
{
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

    struct Configuration
    {
        LoggingConfiguration loggingConfiguration;
        std::string configurationDirectory;
        std::string mainConfigurationFile = "settings.yaml";
    };

    enum class ConfigurationError
    {
        FileNotFound
    };

    template<typename TImplementation>
    class IConfiguration: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        std::string getTextFileContent(const std::filesystem::path& filename, const std::function<void(ConfigurationError)>& errorHandler = nullptr)
        {
            return this->toImplementation()->getTextFileContentImpl(filename, errorHandler);
        }
    };

    class FileSystemBasedConfiguration: public IConfiguration<FileSystemBasedConfiguration>
    {
    private:
        const Configuration& configuration;

    public:
        explicit FileSystemBasedConfiguration(const Configuration& configuration):
                configuration(configuration)
        {}

        std::string getTextFileContentImpl(const std::filesystem::path& filename, const std::function<void(ConfigurationError)>& errorHandler = nullptr)
        {
            auto path = std::filesystem::path{configuration.configurationDirectory} /= filename.c_str();

            auto stream = std::ifstream(path.generic_string());
            stream.exceptions(std::ios_base::badbit);

            if(!stream.is_open())
            {
                if(errorHandler)
                {
                    errorHandler(ConfigurationError::FileNotFound);
                }
                else
                {
                    throw std::ios_base::failure("file does not exist");
                }

                return {};
            }

            auto result = std::string();

            constexpr auto read_size = std::size_t(4096);
            auto buffer = std::string(read_size, '\0');
            while (stream.read(& buffer[0], read_size))
            {
                result.append(buffer, 0, stream.gcount());
            }
            result.append(buffer, 0, stream.gcount());

            return result;
        }
    };
}

REFL_TYPE(dory::configuration::RotationLogSink)
        REFL_FIELD(logFileName)
        REFL_FIELD(maximumFileSize)
        REFL_FIELD(maximumFilesCount)
REFL_END

REFL_TYPE(dory::configuration::StdoutLogSink)
REFL_END

REFL_TYPE(dory::configuration::Logger)
        REFL_FIELD(name)
        REFL_FIELD(rotationLogger)
        REFL_FIELD(stdoutLogger)
REFL_END

REFL_TYPE(dory::configuration::LoggingConfiguration)
        REFL_FIELD(mainLogger)
        REFL_FIELD(configurationLogger)
REFL_END

REFL_TYPE(dory::configuration::Configuration)
        REFL_FIELD(loggingConfiguration)
        REFL_FIELD(configurationDirectory)
        REFL_FIELD(mainConfigurationFile)
REFL_END