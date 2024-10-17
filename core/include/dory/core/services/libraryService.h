#pragma once

#include <dory/core/services/iLibraryService.h>
#include <dory/core/extensionPlatform/dynamicLibrary.h>
#include <map>

namespace dory::core::services
{
    class LibraryService: public ILibraryService
    {
    private:
        std::map<std::string, std::shared_ptr<extensionPlatform::IDynamicLibrary>> _libraries;

    public:
        std::shared_ptr<extensionPlatform::ILibrary> load(const std::string& libraryName, const std::filesystem::path& libraryPath) override;
        void unload(const std::string& libraryName) override;
    };
}
