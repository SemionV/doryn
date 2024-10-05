#pragma once

#include <dory/core/services/iFileService.h>
#include "dory/core/typeTraits.h"

namespace dory::core::services
{
    class FileService: public IFileService
    {
    public:
        std::string read(const std::filesystem::path& filePath) final;
        void write(const std::filesystem::path& filePath, const std::string& content) final;
        std::string getMessage() final;
    };
}
