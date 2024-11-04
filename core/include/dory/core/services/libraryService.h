#pragma once

#include <dory/core/services/iLibraryService.h>
#include <dory/core/extension/dynamicLibrary.h>
#include <map>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class LibraryService: public ILibraryService
    {
    private:
        std::map<std::string, std::shared_ptr<extension::IDynamicLibrary>> _libraries;
        Registry& _registry;

    public:
        explicit LibraryService(Registry& registry);

        std::shared_ptr<extension::IDynamicLibrary> load(const std::string& libraryName, const std::filesystem::path& libraryPath) override;
        bool load(resources::DataContext& context, const resources::configuration::Extension& extensionConfig) override;
        void unload(const std::string& libraryName) override;
        void unloadAll() override;
        bool isLoaded(const std::string& libraryName) override;
    };
}
