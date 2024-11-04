#pragma once

#include <dory/core/repository.h>
#include "resources/program.h"

namespace dory::renderer::opengl
{
    class ProgramRepository: public core::repositories::Repository<resources::Program, GLuint>
    {
    public:
        GLuint getFreeId() override;
    };
}