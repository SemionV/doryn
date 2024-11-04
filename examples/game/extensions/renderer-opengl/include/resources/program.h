#pragma once

#include <glad/gl.h>
#include "id.h"

namespace dory::renderer::opengl::resources
{
    struct Program
    {
        GLuint id = unboundId;
        std::string key;
        Program() = default;

        Program(std::string key):
                key(key)
        {}
    };
}
