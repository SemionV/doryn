#pragma once

#include "../entity.h"
#include <dory/core/resources/graphicalSystem.h>
#include <string>

namespace dory::core::resources::entities
{
    struct ShaderProgram: public Entity<>
    {
        GraphicalSystem graphicalSystem;
        std::string key {};
        IdType windowId {};
    };
}
