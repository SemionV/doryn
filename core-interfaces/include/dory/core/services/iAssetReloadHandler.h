#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/dataContext.h>
#include <filesystem>

namespace dory::core::services
{
    class IAssetReloadHandler: public generic::Interface
    {
    public:
        virtual bool reload(resources::DataContext& context, const std::filesystem::path& filePath) = 0;
    };
}