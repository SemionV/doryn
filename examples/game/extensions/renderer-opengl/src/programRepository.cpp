#include <programRepository.h>

namespace dory::renderer::opengl
{
    void ProgramRepository::setId(resources::Program& program)
    {
        program.id = glCreateProgram();
    }
}