#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"

namespace dory::configuration
{
    struct RotationLogConfiguration
    {
        std::string logFileName = "log.txt";
        std::size_t maximumFileSize = 1048576 * 5;
        std::size_t maximumFilesCount = 3;
    };

    struct LogSinks
    {
        std::optional<RotationLogConfiguration> rotationLogger;
    };

    struct LoggingConfiguration
    {
        std::string logsDirectory = "logs";
        LogSinks sinks;
    };

    struct Configuration
    {
        LoggingConfiguration loggingConfiguration;
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
        const std::filesystem::path configurationPath;

    public:
        explicit FileSystemBasedConfiguration(std::filesystem::path configurationPath):
                configurationPath(std::move(configurationPath))
        {}

        std::string getTextFileContentImpl(const std::filesystem::path& filename, const std::function<void(ConfigurationError)>& errorHandler = nullptr)
        {
            auto path = configurationPath / filename.c_str();

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
