#include <dory/core/registry.h>
#include <openglRenderer.h>
#include <glad/gl.h>

namespace dory::renderer::opengl
{
    OpenglRenderer::OpenglRenderer(core::Registry &registry):
        _registry(registry)
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

    void OpenglRenderer::draw(core::resources::DataContext& context,
                              const core::resources::entities::Window& window,
                              const core::resources::entities::GraphicalContext& graphicalContext,
                              const core::resources::entities::View& view)
    {}
}