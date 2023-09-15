#pragma once

namespace dory
{
    enum class ConfigurationError
    {
        FileNotFound
    };

    class IConfiguration
    {
        public:
            virtual std::string getTextFileContent(const std::filesystem::path& filename, std::function<void(ConfigurationError)> errorHandler = 0) = 0;
    };
}