#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/dataContext.h>
#include <string>
#include <filesystem>
#include <optional>

namespace dory::core::services
{
    class IAssetTypeResolver: public generic::Interface
    {
    public:
        virtual std::optional<std::string> resolve(resources::DataContext& context, const std::filesystem::path& filePath) = 0;
    };
}