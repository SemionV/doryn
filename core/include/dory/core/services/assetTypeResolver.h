#pragma once

#include <dory/core/services/iAssetTypeResolver.h>

namespace dory::core::services
{
    class AssetTypeResolver: public IAssetTypeResolver
    {
    public:
        std::optional<std::string> resolve(resources::DataContext& context, const std::filesystem::path& filePath) override;
    };
}