#pragma once

#include <dory/core/registry.h>

namespace dory::game
{
    class Game
    {
    private:
        core::Registry& _registry;

    public:
        explicit Game(core::Registry& registry):
            _registry(registry)
        {}

        bool initialize(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context)
        {
            core::resources::IdType graphicalContextId {};
            _registry.get<core::repositories::IGraphicalContextRepository>([&graphicalContextId](auto* graphicalContextRepository) {
                auto graphicalContext = graphicalContextRepository->insert(core::resources::entities::GraphicalContext{ {}, core::resources::GraphicalSystem::opengl });
                if(graphicalContext)
                {
                    graphicalContextId = graphicalContext->id;
                }
            });

            _registry.get<dory::core::services::IWindowService>([&context, graphicalContextId](dory::core::services::IWindowService* windowService) {
                context.mainWindowId = windowService->createWindow(core::resources::WindowParameters{800, 600, "dory game", graphicalContextId});
            });

            _registry.get<dory::core::repositories::IViewRepository>([&context](dory::core::repositories::IViewRepository* viewRepository) {
                auto view = dory::core::resources::entities::View{dory::core::resources::nullId, context.mainWindowId};
                viewRepository->insert(view);
            });

            _registry.get<
                    generic::registry::Service<core::services::IAssetService>,
                    generic::registry::Service<core::repositories::IWindowRepository>>(
                [&context](core::services::IAssetService* assetService,
                   core::repositories::IWindowRepository* windowRepository)
            {
                auto shaderProgram = core::resources::objects::ShaderProgram{"triangles", {
                        core::resources::objects::Shader{"openglVertexShader", "configuration/shaders/triangles/triangles.vert"},
                        core::resources::objects::Shader{"openglFragmentShader", "configuration/shaders/triangles/triangles.frag"}
                }};

                auto* window = windowRepository->get(context.mainWindowId);
                if(window)
                {
                    assetService->loadProgram(shaderProgram, *window);
                }
            });

            _registry.get<core::devices::ITerminalDevice>([](core::devices::ITerminalDevice* terminalDevice) {
                terminalDevice->enterCommandMode();
            });

            return true;
        }
    };
}
