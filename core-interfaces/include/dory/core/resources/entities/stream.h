#pragma once

#include <vector>
#include <string>
#include "../entity.h"

namespace dory::core::resources::entities
{
    struct Block
    {
        std::string name;
        std::vector<unsigned char> data;
    };

    struct BlockStream: public Entity<>
    {
        std::string name;
    };
}
