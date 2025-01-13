#pragma once

#include <dory/generic/baseTypes.h>
#include <filesystem>

namespace dory::core::services::files
{
    class IImageFileService: public generic::Interface
    {
    public:
        virtual void save(const std::filesystem::path& filename, const resources::assets::Image& image) = 0;
        virtual resources::assets::Image load(const std::filesystem::path& filename) = 0;
    };
}
