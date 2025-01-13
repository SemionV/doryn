#pragma once

#include <dory/core/resources/entities/stream.h>
#include <dory/core/services/files/iImageFileService.h>

namespace dory::core::services::files
{
    class ImageFileService
    {
    public:
        static void savePng(const std::filesystem::path& filename, const resources::assets::Image& image);
        static void saveBmp(const std::filesystem::path& filename, const resources::assets::Image& image);

        static resources::assets::Image load(const std::filesystem::path& filename);
    };

    class PngImageFileService: public IImageFileService
    {
    public:
        void save(const std::filesystem::path& filename, const resources::assets::Image& image) final;
        resources::assets::Image load(const std::filesystem::path& filename) final;
    };

    class BmpImageFileService: public IImageFileService
    {
    public:
        void save(const std::filesystem::path& filename, const resources::assets::Image& image) final;
        resources::assets::Image load(const std::filesystem::path& filename) final;
    };
}
