#pragma once

#include "../entity.h"
#include <unordered_map>

namespace dory::core::resources::entities
{
    struct GraphicalContextBindings: public Entity<>
    {
        std::unordered_map<resources::IdType, resources::IdType> meshBindings;
    };
}
