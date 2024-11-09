#include <dory/core/registry.h>
#include <dory/core/services/graphicalSystem.h>

namespace dory::core::services
{
    GraphicalSystem::GraphicalSystem(core::Registry& registry):
        _registry(registry)
    {}

    void GraphicalSystem::render(core::resources::DataContext& context, const core::resources::entities::Window& window)
    {
        _registry.get<core::repositories::IViewRepository>([this, &context, &window](core::repositories::IViewRepository* viewRepository) {
            auto viewIds = viewRepository->getWindowViews(window.id);
            auto windowService = _registry.get<core::services::IWindowService>(window.windowSystem);
            auto renderer = _registry.get<core::services::graphics::IRenderer>(window.graphicalSystem);
            for(const auto& viewId : viewIds)
            {
                auto view = viewRepository->get(viewId);

                if(view && windowService && renderer)
                {
                    windowService->setCurrentWindow(window);
                    renderer->draw(context, *view);
                    windowService->swapBuffers(window);
                }
            }
        });
    }

    bool GraphicalSystem::uploadProgram(core::resources::entities::ShaderProgram& program,
                                        const core::resources::entities::Window& window)
    {
        auto shaderService = _registry.get<graphics::IShaderService>(window.graphicalSystem);
        auto windowService = _registry.get<core::services::IWindowService>(window.windowSystem);
        auto logger = _registry.get<core::services::ILogService>();

        if(shaderService && windowService)
        {
            windowService->setCurrentWindow(window);
            if(shaderService->initializeProgram(program))
            {
                auto shaderRepository = _registry.get<repositories::IShaderRepository>(window.graphicalSystem);
                auto fileService = _registry.get<services::IFileService>();
                if(shaderRepository && fileService)
                {
                    auto shaders = shaderRepository->getAll();
                    for(auto& shader : shaders)
                    {
                        auto sourceCode = fileService->read(shader.filePath);
                        if(!shaderService->initializeShader(program, shader, sourceCode) && logger)
                        {
                            logger->error("Cannot initialize shader: " + program.key + ", " + shader.filePath.string());
                        }
                    }
                }
            }
            else
            {
                if(logger)
                {
                    logger->error("Cannot initialize shader program: " + program.key);
                }

                return false;
            }
        }

        return true;
    }
}