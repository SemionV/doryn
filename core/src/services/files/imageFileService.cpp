#include <dory/core/services/files/imageFileService.h>
#include <dory/core/resources/assets/image.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace dory::core::services::files
{
    void ImageFileService::savePng(const std::filesystem::path& filename, const resources::assets::Image& image)
    {
        stbi_write_png(filename.c_str(),
            image.width,
            image.height,
            image.components,
            image.data.data(),
            image.width * image.components);
    }

    resources::assets::Image ImageFileService::load(const std::filesystem::path& filename)
    {
        resources::assets::Image image;

        int width, height, channels;

        unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &channels, 0);
        if (imageData)
        {
            const size_t totalSize = width * height * channels;

            image.components = channels;
            image.width = width;
            image.height = height;
            image.data = std::vector<unsigned char> { imageData, imageData + totalSize };
        }

        return image;
    }

    void ImageFileService::saveBmp(const std::filesystem::path& filename, const resources::assets::Image& image)
    {
        auto result = stbi_write_bmp(filename.c_str(),
            image.width,
            image.height,
            image.components,
            image.data.data());
    }

    void PngImageFileService::save(const std::filesystem::path& filename, const resources::assets::Image& image)
    {
        ImageFileService::savePng(filename, image);
    }

    resources::assets::Image PngImageFileService::load(const std::filesystem::path& filename)
    {
        return ImageFileService::load(filename);
    }

    void BmpImageFileService::save(const std::filesystem::path& filename, const resources::assets::Image& block)
    {
        ImageFileService::saveBmp(filename, block);
    }

    resources::assets::Image BmpImageFileService::load(const std::filesystem::path& filename)
    {
        return ImageFileService::load(filename);
    }
}
