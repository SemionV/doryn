#pragma once

#include <vector>
#include <string>
#include "../entity.h"
#include "../assets/image.h"
#include <dory/generic/stream.h>

namespace dory::core::resources::entities
{
    enum class BlockType
    {
        image
    };

    struct Block
    {
        std::string name;
        BlockType blockType;
    };

    struct BlockStream: public Entity<>
    {
        std::string name;
        BlockType blockType;
    };

    class ImageStream: public generic::IStream<assets::Image>, public Entity<>
    {};
}
