#pragma once

#include "../entity.h"
#include <unordered_map>

namespace dory::core::resources::entities
{
    struct GraphicalContext: public Entity<>
    {
        std::unordered_map<resources::IdType, resources::IdType> meshBindings;
    };
}
