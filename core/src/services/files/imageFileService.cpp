#include <dory/core/services/files/imageFileService.h>
#include <dory/core/resources/blockStreamQueue.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace dory::core::services::files
{
    void ImageFileService::savePng(const std::filesystem::path& filename, const resources::entities::Block& block)
    {
        assert(block.blockType == resources::entities::BlockType::image);

        auto& imageBlock = static_cast<const resources::entities::ImageBlock&>(block);

        stbi_write_png(filename.c_str(),
            imageBlock.width,
            imageBlock.height,
            imageBlock.components,
            imageBlock.data.data(),
            imageBlock.width * imageBlock.components);
    }

    resources::entities::Block ImageFileService::load(const std::filesystem::path& filename)
    {
        resources::entities::ImageBlock imageBlock;

        int width, height, channels;

        unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &channels, 0);
        if (imageData)
        {
            const size_t totalSize = width * height * channels;

            imageBlock.components = channels;
            imageBlock.width = width;
            imageBlock.height = height;
            imageBlock.data = std::vector<unsigned char> { imageData, imageData + totalSize };
        }

        return imageBlock;
    }

    void ImageFileService::saveBmp(const std::filesystem::path& filename, const resources::entities::Block& block)
    {
        assert(block.blockType == resources::entities::BlockType::image);

        auto& imageBlock = static_cast<const resources::entities::ImageBlock&>(block);

        stbi_write_bmp(filename.c_str(),
            imageBlock.width,
            imageBlock.height,
            imageBlock.components,
            imageBlock.data.data());
    }

    void PngImageFileService::save(const std::filesystem::path& filename, const resources::entities::Block& block)
    {
        savePng(filename, block);
    }

    resources::entities::Block PngImageFileService::load(const std::filesystem::path& filename)
    {
        return load(filename);
    }

    void BmpImageFileService::save(const std::filesystem::path& filename, const resources::entities::Block& block)
    {
        saveBmp(filename, block);
    }

    resources::entities::Block BmpImageFileService::load(const std::filesystem::path& filename)
    {
        return load(filename);
    }
}
