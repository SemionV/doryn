#include <dory/core/registry.h>
#include <openglRenderer.h>
#include <glad/gl.h>

namespace dory::renderer::opengl
{
    OpenglRenderer::OpenglRenderer(core::Registry &registry, ShaderProgramService& shaderProgramService):
        _registry(registry),
        _shaderProgramService(shaderProgramService)
    {}

    void OpenglRenderer::draw(core::resources::DataContext& context, const core::resources::entities::View& view)
    {
        if(glClearColor)
        {
            glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();
        }
    }
}