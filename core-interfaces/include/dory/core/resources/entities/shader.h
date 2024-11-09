#pragma once

#include "../entity.h"
#include "../graphicalSystem.h"

namespace dory::core::resources::entities
{
    struct ShaderType
    {
        const constexpr static std::string_view glFragmentShader = "openglFragmentShader";
        const constexpr static std::string_view glVertexShader = "openglVertexShader";
    };

    struct Shader: Entity<>
    {
        std::filesystem::path filePath;
        IdType programId {};
        std::string type;
    };
}
