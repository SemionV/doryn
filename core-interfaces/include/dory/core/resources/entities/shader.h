#pragma once

#include "../entity.h"
#include "../graphicalSystem.h"

namespace dory::core::resources::entities
{
    struct Shader: Entity<>
    {
        std::filesystem::path filePath;
        IdType programId {};
    };
}
