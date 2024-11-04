#include <programRepository.h>

namespace dory::renderer::opengl
{

    GLuint ProgramRepository::getFreeId()
    {
        return glCreateProgram();
    }
}