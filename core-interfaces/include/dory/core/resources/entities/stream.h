#pragma once

#include <filesystem>
#include "../entity.h"
#include "../assets/image.h"
#include <dory/core/resources/systemTypes.h>
#include <dory/generic/stream.h>

namespace dory::core::resources::entities
{
    class ImageStream: public generic::IStream<assets::Image>, public Entity<>
    {
    public:
        std::filesystem::path destinationDirectory {};
        AssetFileFormat fileFormat = AssetFileFormat::bmp;
        std::size_t maxImagesCount {};
    };
}
