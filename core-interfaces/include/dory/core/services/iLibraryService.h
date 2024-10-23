#pragma once

#include <memory>
#include <filesystem>
#include <dory/core/extensionPlatform/iDynamicLibrary.h>

namespace dory::core::services
{
    class ILibraryService
    {
    public:
        virtual ~ILibraryService() = default;

        virtual std::shared_ptr<extensionPlatform::IDynamicLibrary> load(const std::string& libraryName, const std::filesystem::path& libraryPath) = 0;
        virtual void unload(const std::string& libraryName) = 0;
    };
}