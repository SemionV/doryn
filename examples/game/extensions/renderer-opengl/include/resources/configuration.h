#pragma once

#include <string>
#include <vector>

namespace dory::renderer::opengl::resources::configuration
{
    struct Shader
    {
        std::string filename;
        std::string type;
    };

    struct Program
    {
        std::string key;
        std::vector<Shader> shaders;
    };
}
