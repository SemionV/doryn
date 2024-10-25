#pragma once

#include <string>
#include <filesystem>

namespace dory::core::services
{
    class IFileService
    {
    public:
        virtual ~IFileService() = default;
        virtual std::string read(const std::filesystem::path& filePath) = 0;
        virtual void write(const std::filesystem::path& filePath, const std::string& content) = 0;
    };
}