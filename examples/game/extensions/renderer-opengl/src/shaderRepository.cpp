#include <shaderRepository.h>

namespace dory::renderer::opengl
{
    void ShaderRepository::setId(resources::Shader& shader)
    {
        shader.id = glCreateShader(shader.type);
    }
}