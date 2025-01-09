#pragma once

#include <vector>
#include <string>
#include "../entity.h"

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
}
