#pragma once

#include <glad/gl.h>
#include <string>
#include "id.h"

namespace dory::renderer::opengl::resources
{
    struct Program
    {
        GLuint id = unboundId;
        std::string key;
        Program() = default;

        explicit Program(std::string  key):
                key(std::move(key))
        {}
    };
}
