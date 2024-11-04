#pragma once

#include <dory/core/repository.h>
#include "resources/shader.h"
#include "resources/program.h"

namespace dory::renderer::opengl
{
    class ShaderRepository: public core::repositories::Repository<resources::Shader, GLuint>
    {};
}
