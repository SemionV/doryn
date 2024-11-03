#pragma once

#include <dory/core/services/iAssetLoader.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services::loaders
{
    class ExtensionLoader: public IAssetLoader
    {
    private:
        Registry& _registry;

    public:
        explicit ExtensionLoader(Registry& registry);

        bool load(resources::DataContext& context, const std::filesystem::path& filePath) override;
    };
}
