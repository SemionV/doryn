#pragma once

#include "configuration.h"
#include "base/doryExport.h"

namespace dory::configuration
{
    class DORY_API FileSystemBasedConfiguration
    {
        private:
            std::filesystem::path configurationPath;

        public:
            explicit FileSystemBasedConfiguration(std::filesystem::path configurationPath);

            std::string getTextFileContent(const std::filesystem::path& filename, const std::function<void(ConfigurationError)>& errorHandler = 0) const;
    };
}