#pragma once

#include <dory/generic/baseTypes.h>
#include <filesystem>

namespace dory::core::services::files
{
    class IImageFileService: public generic::Interface
    {
    public:
        virtual void save(const std::filesystem::path& filename, const resources::entities::Block& block) = 0;
        virtual resources::entities::Block load(const std::filesystem::path& filename) = 0;
    };
}
