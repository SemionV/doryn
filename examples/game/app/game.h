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

            core::resources::entities::Window* mainWindow {};

            _registry.get<dory::core::services::IWindowService>([this, &context, &graphicalContext, &mainWindow](dory::core::services::IWindowService* windowService) {
                auto windowParameters = core::resources::WindowParameters{ 800, 600, "dory game", graphicalContext->id };
                auto window = windowService->createWindow(windowParameters, core::resources::WindowSystem::glfw);
                if(window)
                {
                    context.mainWindowId = window->id;
                    mainWindow = window;

                    auto viewService = _registry.get<core::services::IViewService>();
                    if(viewService)
                    {
                        auto view = viewService->createView(*window);
                        if(view)
                        {
                            view->viewport = { 0, 0, windowParameters.width, windowParameters.height };
                        }
                    }
                }
            });

            _registry.get<core::events::window::Bundle::IListener>([this](core::events::window::Bundle::IListener* listener){
                listener->attach([this](auto& context, const core::events::window::Resize& event){
                    auto windowRepository = _registry.get<core::repositories::IWindowRepository>();
                    auto viewRepository = _registry.get<core::repositories::IViewRepository>();
                    if(windowRepository && viewRepository)
                    {
                        auto window = windowRepository->get(event.windowId);
                        if(window)
                        {
                            window->width = event.width;
                            window->height = event.height;

                            if(window->views.size() == 1)
                            {
                                auto view = viewRepository->get(window->views[0]);
                                if(view)
                                {
                                    view->viewport.width = event.width;
                                    view->viewport.height = event.height;
                                }
                            }
                        }
                    }
                });
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
            auto materialGouraudId = core::resources::nullId;

            auto windowService = _registry.get<core::services::IWindowService>();
            if(windowService)
            {
                windowService->setCurrentWindow(*mainWindow);
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
                    { {1.f, 1.f, 1.f, 0.f}, {1.f, 0.f, 0.f, 0.f}, {0.f, 1.f, 0.f, 0.f} },
                    core::resources::assets::PolygonMode::Wireframe
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

            if(shaderRepo && materialRepo)
            {
                auto vertexShader = shaderRepo->insert(core::resources::assets::Shader{});
                vertexShader->type = core::resources::assets::ShaderType::vertex;
                vertexShader->filename = "configuration/shaders/triangles/gouraud.vert";
                vertexShader->sourceCode = fileService->read(vertexShader->filename);
                auto vertexShaderId = vertexShader->id;

                auto fragmentShader = shaderRepo->insert(core::resources::assets::Shader{});
                fragmentShader->type = core::resources::assets::ShaderType::fragment;
                fragmentShader->filename = "configuration/shaders/triangles/gouraud.frag";
                fragmentShader->sourceCode = fileService->read(fragmentShader->filename);
                auto fragmentShaderId = fragmentShader->id;

                auto material = materialRepo->insert(core::resources::assets::Material{
                    {},
                    { vertexShaderId, fragmentShaderId }
                });
                materialGouraudId = material->id;

                auto assetBinder = _registry.get<core::services::graphics::IAssetBinder>(core::resources::AssetTypeName::shader);
                if(assetBinder)
                {
                    assetBinder->bind(vertexShaderId, *graphicalContext);
                    assetBinder->bind(fragmentShaderId, *graphicalContext);
                }

                assetBinder = _registry.get<core::services::graphics::IAssetBinder>(core::resources::AssetTypeName::material);
                if(assetBinder)
                {
                    assetBinder->bind(materialGouraudId, *graphicalContext);
                }
            }

            auto meshRepo = _registry.get<core::repositories::assets::IMeshRepository>();
            if(meshRepo)
            {
                auto mesh = meshRepo->insert(core::resources::assets::Mesh{});
                mesh->materialId = materialGouraudId;
                mesh->positions.componentsCount = 2;
                mesh->positions.components = {-1.f, 0.f,  0.f, -1.f,  0.f, 1.f,};
                mesh->vertexCount = mesh->positions.components.size() / mesh->positions.componentsCount;
                mesh->colors.componentsCount = 4;
                mesh->colors.components = {1.f,0.f,0.f,1.f, 0.f,1.f,0.f,1.f, 0.f,0.f,1.f,1.f};

                auto assetBinder = _registry.get<core::services::graphics::IAssetBinder>(core::resources::AssetTypeName::mesh);
                if(assetBinder)
                {
                    windowService->setCurrentWindow(*mainWindow);
                    assetBinder->bind(mesh->id, *graphicalContext);
                }
            }

            auto meshGenerator = _registry.get<core::services::generators::IMeshGenerator>();
            if(meshRepo && meshGenerator)
            {
                auto mesh = meshRepo->insert(core::resources::assets::Mesh{});
                //meshGenerator->triangle(1.f, 0.3f, *mesh);
                //meshGenerator->rectangle(1.f, 1.f, *mesh);
                meshGenerator->cube(0.5f, *mesh);
                mesh->materialId = materialId;

                auto assetBinder = _registry.get<core::services::graphics::IAssetBinder>(core::resources::AssetTypeName::mesh);
                if(assetBinder)
                {
                    windowService->setCurrentWindow(*mainWindow);
                    assetBinder->bind(mesh->id, *graphicalContext);
                }
            }

            return true;
        }
    };
}
