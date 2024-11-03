#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/dataContext.h>
#include <filesystem>

namespace dory::core::services
{
    class IAssetLoader: public generic::Interface
    {
    public:
        virtual bool load(resources::DataContext& context, const std::filesystem::path& filePath) = 0;
    };
}