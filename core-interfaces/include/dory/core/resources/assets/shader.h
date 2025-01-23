#pragma once

#include "../entity.h"
#include <filesystem>

namespace dory::core::resources::assets
{
    enum class ShaderType
    {
        vertex,
        fragment
    };

    struct Shader: Entity<>
    {
        ShaderType type {};
        std::string sourceCode {};
        std::filesystem::path filename {};
    };
}