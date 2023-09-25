#pragma once

#include "configuration.h"
#include "base/doryExport.h"

namespace dory::configuration
{
    class DORY_API FileSystemBasedConfiguration: public IConfiguration
    {
        private:
            std::filesystem::path configurationPath;

        public:
            FileSystemBasedConfiguration(std::filesystem::path configurationPath);

            std::string getTextFileContent(const std::filesystem::path& filename, std::function<void(ConfigurationError)> errorHandler = 0) override;
    };
}