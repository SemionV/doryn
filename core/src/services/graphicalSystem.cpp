#include <dory/core/registry.h>
#include <dory/core/services/graphicalSystem.h>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::bindings;
    using namespace resources::entities;
    using namespace resources::assets;
    using namespace repositories;
    using namespace repositories::bindings;
    using namespace repositories::assets;
    using namespace services;
    using namespace services::graphics;

    GraphicalSystem::GraphicalSystem(core::Registry& registry):
        _registry(registry)
    {}

    void GraphicalSystem::render(DataContext& context, const Window& window)
    {
        _registry.get<IViewRepository>([this, &context, &window](IViewRepository* viewRepository) {
            auto viewIds = viewRepository->getWindowViews(window.id);
            auto windowService = _registry.get<IWindowService>(window.windowSystem);
            auto renderer = _registry.get<IRenderer>(window.graphicalSystem);
            for(const auto& viewId : viewIds)
            {
                auto view = viewRepository->get(viewId);

                if(view && windowService && renderer)
                {
                    windowService->setCurrentWindow(window.id);
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
            windowService->setCurrentWindow(window.id);
            if(shaderService->initializeProgram(program))
            {
                auto shaderRepository = _registry.get<repositories::IShaderRepository>(window.graphicalSystem);
                auto fileService = _registry.get<services::IFileService>();
                if(shaderRepository && fileService)
                {
                    shaderRepository->each([&fileService, &program, &shaderService, &logger](auto& shader) {
                        auto sourceCode = fileService->read(shader.filePath);
                        if(!shaderService->initializeShader(program, shader, sourceCode) && logger)
                        {
                            logger->error("Cannot initialize shader: " + program.key + ", " + shader.filePath.string());
                        }
                    });
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