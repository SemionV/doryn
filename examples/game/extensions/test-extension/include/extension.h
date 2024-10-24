#pragma once

#include "dory/generic/extension/iModule.h"
#include <dory/generic/extension/registryResourceScope.h>
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

    class Extension: public dory::generic::extension::IModule<dory::core::resources::DataContext>
    {
    private:
        core::Registry& _registry;
        core::RegistryResourceScope<dory::core::services::IFileService> _fileService;

    public:
        explicit Extension(core::Registry& registry);

        ~Extension() override;

        void attach(dory::generic::extension::LibraryHandle library, dory::core::resources::DataContext& dataContext) final;
    };
}