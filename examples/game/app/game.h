#pragma once

#include <dory/core/registry.h>
#include <dory/core/resources/scene/components.h>
#include "controllers/animationController.h"
#include "controllers/transformController.h"

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
            core::resources::entities::View* mainView {};

            _registry.get<dory::core::services::IWindowService>([&](dory::core::services::IWindowService* windowService) {
                auto windowParameters = core::resources::WindowParameters{ 800, 600, "dory game", graphicalContext->id, 16, true, false };
                auto window = windowService->createWindow(windowParameters, core::resources::WindowSystem::glfw);
                if(window)
                {
                    context.mainWindowId = window->id;
                    mainWindow = window;

                    auto viewService = _registry.get<core::services::IViewService>();
                    if(viewService)
                    {
                        auto view = mainView = viewService->createView(*window);
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

            _registry.get<dory::core::repositories::IPipelineRepository>([this, &libraryHandle, &context](dory::core::repositories::IPipelineRepository* pipelineRepository){
                auto animationController = std::make_shared<AnimationController>(_registry);
                auto controllerHandle = generic::extension::ResourceHandle<core::resources::entities::PipelineNode::ControllerPointerType>{ libraryHandle, animationController };
                pipelineRepository->addNode(core::resources::entities::PipelineNode{controllerHandle, context.inputGroupNodeId});

                auto transformController = std::make_shared<TransformController>(_registry);
                controllerHandle = generic::extension::ResourceHandle<core::resources::entities::PipelineNode::ControllerPointerType>{ libraryHandle, transformController };
                pipelineRepository->addNode(core::resources::entities::PipelineNode{controllerHandle, context.inputGroupNodeId});
            });

            _registry.get<core::devices::ITerminalDevice>([](core::devices::ITerminalDevice* terminalDevice) {
                terminalDevice->enterCommandMode();
            });

            //Test scene
            loadAssets(context, mainWindow, graphicalContext);
            auto scene = buildScene(context);
            if(scene)
            {
                mainView->sceneId = scene->id;
                mainView->sceneEcsType = scene->ecsType;
            }

            return true;
        }

        void loadAssets(core::resources::DataContext& context, core::resources::entities::Window* window, core::resources::entities::GraphicalContext* graphicalContext)
        {
            auto shaderRepo = _registry.get<core::repositories::assets::IShaderRepository>();
            auto materialRepo = _registry.get<core::repositories::assets::IMaterialRepository>();
            auto fileService = _registry.get<core::services::IFileService>();
            auto materialId = core::resources::nullId;
            auto materialGouraudId = core::resources::nullId;

            auto windowService = _registry.get<core::services::IWindowService>();
            if(windowService)
            {
                windowService->setCurrentWindow(*window);
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
                    windowService->setCurrentWindow(*window);
                    assetBinder->bind(mesh->id, *graphicalContext);
                }
            }

            auto meshGenerator = _registry.get<core::services::generators::IMeshGenerator>();
            if(meshRepo && meshGenerator)
            {
                auto mesh = meshRepo->insert(core::resources::assets::Mesh{});
                meshRepo->setName(mesh->id, "cube");
                //meshGenerator->triangle(1.f, 1.f, *mesh);
                //meshGenerator->rectangle(0.5f, 0.5f, *mesh);
                meshGenerator->cube(0.5f, *mesh);
                mesh->materialId = materialId;

                /*mesh->colors.componentsCount = 4;
                mesh->colors.components = {1.f,0.f,0.f,1.f, 0.f,1.f,0.f,1.f, 0.f,0.f,1.f,1.f};*/

                auto assetBinder = _registry.get<core::services::graphics::IAssetBinder>(core::resources::AssetTypeName::mesh);
                if(assetBinder)
                {
                    windowService->setCurrentWindow(*window);
                    assetBinder->bind(mesh->id, *graphicalContext);
                }
            }
        }

        core::resources::scene::Scene* buildScene(core::resources::DataContext& context)
        {
            auto meshRepo = _registry.get<core::repositories::assets::IMeshRepository>();
            auto sceneRepo = _registry.get<core::repositories::ISceneRepository>();
            auto sceneService = _registry.get<core::services::ISceneService>();

            if(meshRepo && sceneRepo && sceneService)
            {
                auto scene = sceneRepo->insert(core::resources::scene::Scene{ {}, "main scene" });

                auto cubeMeshId = meshRepo->getId("cube");
                glm::mat4 rotationMatrix = glm::mat4x4{ 1 };
                rotationMatrix = glm::rotate(rotationMatrix, glm::radians(45.f), glm::vec3(1.0f, 0.0f, 0.0f));
                rotationMatrix = glm::rotate(rotationMatrix, glm::radians(45.f), glm::vec3(0.0f, 0.0f, 1.0f));
                //TODO: use proper material id(get by material name)
                auto cubeObject = core::resources::objects::SceneObject { "the cube", core::resources::nullId, cubeMeshId, 1, { {0.f, 0.f, 0.f}, glm::quat_cast(rotationMatrix) }};

                auto cubeObjectId = sceneService->addObject(*scene, cubeObject);
                sceneService->addComponent(cubeObjectId, *scene,
                                           core::resources::scene::components::Rotation{ glm::radians(10.f), glm::normalize(glm::vec3{0.f, 1.f, 0.f}) });
                sceneService->addComponent(cubeObjectId, *scene,
                                           core::resources::scene::components::Translation{ 0.03f, glm::normalize(glm::vec3{1.f, 0.f, 0.f}) });


                return scene;
            }

            return nullptr;
        }
    };
}
