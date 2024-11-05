#pragma once

#include <dory/core/repository.h>
#include "resources/shader.h"

namespace dory::renderer::opengl
{
    class ShaderRepository: public core::repositories::Repository<resources::Shader, GLuint>
    {
    public:
        void setId(resources::Shader& shader) override;
    };
}
