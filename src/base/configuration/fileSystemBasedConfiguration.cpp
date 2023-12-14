#include "base/dependencies.h"
#include "fileSystemBasedConfiguration.h"

namespace dory::configuration
{
    FileSystemBasedConfiguration::FileSystemBasedConfiguration(std::filesystem::path configurationPath):
        configurationPath(std::move(configurationPath))
    {
    }

    std::string FileSystemBasedConfiguration::getTextFileContent(const std::filesystem::path& filename, const std::function<void(ConfigurationError)>& errorHandler) const
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
}