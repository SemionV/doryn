#pragma once

#include <dory/core/services/iAssetReloadHandler.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services::loaders
{
    class ExtensionLoader: public IAssetReloadHandler
    {
    private:
        Registry& _registry;

    public:
        explicit ExtensionLoader(Registry& registry);

        bool reload(resources::DataContext& context, const std::filesystem::path& filePath) override;
    };
}
