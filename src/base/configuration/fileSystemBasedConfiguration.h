#pragma once

#include <utility>

#include "base/domain/configuration.h"
#include "base/doryExport.h"
#include "base/typeComponents.h"

namespace dory::configuration
{
    class DORY_API FileSystemBasedConfiguration: Uncopyable
    {
        private:
            std::filesystem::path configurationPath;

        public:
            explicit FileSystemBasedConfiguration(std::filesystem::path configurationPath);

            std::string getTextFileContent(const std::filesystem::path& filename, const std::function<void(ConfigurationError)>& errorHandler = 0) const;
    };
}