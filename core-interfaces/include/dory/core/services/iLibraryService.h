#pragma once

#include <memory>
#include <filesystem>
#include <dory/core/extensionPlatform/iLibrary.h>

namespace dory::core::services
{
    class ILibraryService
    {
    public:
        virtual ~ILibraryService() = default;

        virtual std::shared_ptr<extensionPlatform::ILibrary> load(const std::string& libraryName, const std::filesystem::path& libraryPath) = 0;
        virtual void unload(const std::string& libraryName) = 0;
    };
}