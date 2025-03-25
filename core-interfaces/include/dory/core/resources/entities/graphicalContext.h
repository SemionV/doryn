#pragma once

#include "../entity.h"
#include <dory/core/resources/systemTypes.h>
#include <unordered_map>

namespace dory::core::resources::entities
{
    struct GraphicalContext: public Entity<>
    {
        GraphicalSystem graphicalSystem;
        std::unordered_map<resources::IdType, resources::IdType> meshBindings;
        std::unordered_map<resources::IdType, resources::IdType> shaderBindings;
        std::unordered_map<resources::IdType, resources::IdType> materialBindings;
    };
}
