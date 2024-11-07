#pragma once

#include "../entity.h"
#include "../graphicalSystem.h"

namespace dory::core::resources::entities
{
    struct Shader: Entity<>
    {
        GraphicalSystem graphicalSystem;
        std::filesystem::path filePath;
    };
}
