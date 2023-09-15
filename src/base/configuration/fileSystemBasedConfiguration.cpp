#include "base/dependencies.h"
#include "fileSystemBasedConfiguration.h"

namespace dory
{
    FileSystemBasedConfiguration::FileSystemBasedConfiguration(std::filesystem::path configurationPath):
        configurationPath(configurationPath)
    {
    }

    std::string FileSystemBasedConfiguration::getTextFileContent(const std::filesystem::path& filename, std::function<void(ConfigurationError)> errorHandler)
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

            return std::string();
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
}