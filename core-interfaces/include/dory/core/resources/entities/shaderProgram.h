#pragma once

#include "../entity.h"

namespace dory::core::resources::entities
{
    struct ShaderProgram: public Entity<>
    {
        std::string key {};
        IdType viewId {};
    };
}
