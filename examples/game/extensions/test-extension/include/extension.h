#pragma once

#include <dory/core/extensionPlatform/iModule.h>
#include <dory/core/extensionPlatform/registryResourceScope.h>
#include <dory/core/registry.h>

namespace dory::game::test_extension
{
    class FileService: public core::services::IFileService
    {
    public:
        std::string read(const std::filesystem::path& filePath) final;

        void write(const std::filesystem::path& filePath, const std::string& content) final;

        std::string getMessage() final;
    };

    class Extension: public dory::core::extensionPlatform::IModule
    {
    private:
        core::Registry& _registry;
        core::extensionPlatform::RegistryResourceScope<dory::core::services::IFileService> _fileService;

    public:
        explicit Extension(core::Registry& registry);

        ~Extension() override;

        void attach(dory::core::extensionPlatform::LibraryHandle library, dory::core::resources::DataContext& dataContext) final;
    };
}