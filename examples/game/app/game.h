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
            core::resources::entities::GraphicalContext* graphicalContext = nullptr;
            _registry.get<core::repositories::IGraphicalContextRepository>([&graphicalContext](auto* graphicalContextRepository) {
                graphicalContext = graphicalContextRepository->insert(core::resources::entities::GraphicalContext{ {}, core::resources::GraphicalSystem::opengl });
            });

            _registry.get<dory::core::services::IWindowService>([&context, &graphicalContext](dory::core::services::IWindowService* windowService) {
                context.mainWindowId = windowService->createWindow(core::resources::WindowParameters{800, 600, "dory game", graphicalContext->id});
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

            //Test scene

            auto shaderRepo = _registry.get<core::repositories::assets::IShaderRepository>();
            auto materialRepo = _registry.get<core::repositories::assets::IMaterialRepository>();
            auto fileService = _registry.get<core::services::IFileService>();
            auto materialId = core::resources::nullId;

            auto windowService = _registry.get<core::services::IWindowService>();
            if(windowService)
            {
                windowService->setCurrentWindow(context.mainWindowId);
            }

            if(shaderRepo && materialRepo)
            {
                auto vertexShader = shaderRepo->insert(core::resources::assets::Shader{});
                vertexShader->type = core::resources::assets::ShaderType::vertex;
                vertexShader->filename = "configuration/shaders/triangles/triangles.vert";
                vertexShader->sourceCode = fileService->read(vertexShader->filename);
                auto vertexShaderId = vertexShader->id;

                auto fragmentShader = shaderRepo->insert(core::resources::assets::Shader{});
                fragmentShader->type = core::resources::assets::ShaderType::fragment;
                fragmentShader->filename = "configuration/shaders/triangles/triangles.frag";
                fragmentShader->sourceCode = fileService->read(fragmentShader->filename);
                auto fragmentShaderId = fragmentShader->id;

                auto material = materialRepo->insert(core::resources::assets::Material{
                    {},
                    { vertexShaderId, fragmentShaderId },
                    { {1.f, 0.f, 1.f, 0.f}, {1.f, 0.f, 0.f, 0.f}, {0.f, 1.f, 0.f, 0.f} }
                });
                materialId = material->id;

                auto assetBinder = _registry.get<core::services::graphics::IAssetBinder>(core::resources::AssetTypeName::shader);
                if(assetBinder)
                {
                    assetBinder->bind(vertexShaderId, *graphicalContext);
                    assetBinder->bind(fragmentShaderId, *graphicalContext);
                }

                assetBinder = _registry.get<core::services::graphics::IAssetBinder>(core::resources::AssetTypeName::material);
                if(assetBinder)
                {
                    assetBinder->bind(materialId, *graphicalContext);
                }
            }

            auto meshRepo = _registry.get<core::repositories::assets::IMeshRepository>();
            if(meshRepo)
            {
                auto mesh = meshRepo->insert(core::resources::assets::Mesh{});
                mesh->materialId = materialId;
                mesh->positions.componentsCount = 2;
                mesh->positions.components = {/*-1.f, 1.f,  0.f, 0.f,  1.f, 1.f,
                                              0.f, 0.f, -1.f, -1.f,  1.f, -1.f,*/
                                              0.f, 1.f, 0.f, -1.f, 1.f, 0.f,};
                mesh->vertexCount = mesh->positions.components.size() / mesh->positions.componentsCount;
                //mesh->indices = {0, 1, 2, 1, 3, 4};

                auto assetBinder = _registry.get<core::services::graphics::IAssetBinder>(core::resources::AssetTypeName::mesh);
                if(assetBinder)
                {
                    windowService->setCurrentWindow(context.mainWindowId);
                    assetBinder->bind(mesh->id, *graphicalContext);
                }
            }

            return true;
        }
    };
}
