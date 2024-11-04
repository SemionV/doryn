#include <shaderRepository.h>

namespace dory::renderer::opengl
{

    GLuint ShaderRepository::getFreeId()
    {
        return glCreateShader();
    }
}