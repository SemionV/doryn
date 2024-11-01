#include <dory/core/registry.h>
#include <dory/core/services/openglRenderer.h>
#include <dory/core/services/iGraphicalSystem.h>
#include <dory/core/resources/graphicalSystem.h>
#include <glad/gl.h>

namespace dory::core::services
{
    OpenglRenderer::OpenglRenderer(Registry &registry):
        _registry(registry)
    {}

    bool OpenglRenderer::initialize(resources::DataContext& context)
    {
        return true;
    }

    void OpenglRenderer::draw(resources::DataContext& context, const resources::entity::Window& window, const resources::entity::View& view)
    {
        _registry.get<IGraphicalSystem, resources::GraphicalSystem::opengl>([&window, &view](IGraphicalSystem* graphicalSystem) {
            graphicalSystem->setCurrentWindow(window);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();
            graphicalSystem->swapBuffers(window);
        });
    }
}