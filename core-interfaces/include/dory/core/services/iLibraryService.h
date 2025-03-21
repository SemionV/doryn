#pragma once

#include <memory>
#include <filesystem>
#include <dory/core/extension/iDynamicLibrary.h>

namespace dory::core::services
{
    class ILibraryService
    {
    public:
        virtual ~ILibraryService() = default;

        virtual std::shared_ptr<extension::IDynamicLibrary> load(const std::string& libraryName, const std::filesystem::path& libraryPath) = 0;
        virtual bool load(resources::DataContext& context, const resources::configuration::Extension& extensionConfig) = 0;
        virtual void unload(const std::string& libraryName) = 0;
        virtual void unloadAll() = 0;
        virtual bool isLoaded(const std::string& libraryName) = 0;
    };
}