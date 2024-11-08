#pragma once

namespace dory::core::resources::objects
{
    struct Shader
    {
        std::string type;
        std::string fileName;
    };

    struct ShaderProgram
    {
        std::string key;
        std::vector<Shader> shaders;
    };
}
