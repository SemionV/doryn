#pragma once

#include <dory/core/resources/entities/stream.h>
#include <dory/core/services/files/iImageFileService.h>

namespace dory::core::services::files
{
    class ImageFileService
    {
    protected:
        void savePng(const std::filesystem::path& filename, const resources::entities::Block& block);
        void saveBmp(const std::filesystem::path& filename, const resources::entities::Block& block);

        resources::entities::Block load(const std::filesystem::path& filename);
    };

    class PngImageFileService: public ImageFileService, public IImageFileService
    {
    public:
        void save(const std::filesystem::path& filename, const resources::entities::Block& block) final;
        resources::entities::Block load(const std::filesystem::path& filename) final;
    };

    class BmpImageFileService: public ImageFileService, public IImageFileService
    {
    public:
        void save(const std::filesystem::path& filename, const resources::entities::Block& block) final;
        resources::entities::Block load(const std::filesystem::path& filename) final;
    };
}
