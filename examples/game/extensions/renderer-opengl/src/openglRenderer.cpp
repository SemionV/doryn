#include <dory/core/registry.h>
#include <openglRenderer.h>
#include <dory/core/services/iGraphicalSystem.h>
#include <dory/core/resources/graphicalSystem.h>
#include <glad/gl.h>

namespace dory::renderer::opengl
{
    OpenglRenderer::OpenglRenderer(core::Registry &registry, ShaderProgramService& shaderProgramService):
        _registry(registry),
        _shaderProgramService(shaderProgramService)
    {}

    bool OpenglRenderer::initialize(core::resources::DataContext& context)
    {
        /*if(context.configuration.materials.contains("opengl-default"))
        {
            core::resources::configuration::RenderingMaterial material = context.configuration.materials["opengl-default"];

            _shaderProgramService.loadProgram(material.program);
        }*/

        return true;
    }

    void OpenglRenderer::draw(core::resources::DataContext& context, const core::resources::entity::Window& window, const core::resources::entity::View& view)
    {
        //TODO: load materials

        _registry.get<core::services::IGraphicalSystem, core::resources::GraphicalSystem::opengl>([&window, &view](core::services::IGraphicalSystem* graphicalSystem) {
            graphicalSystem->setCurrentWindow(window);
            glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();
            graphicalSystem->swapBuffers(window);
        });
    }
}