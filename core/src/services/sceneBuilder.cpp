#include <dory/core/registry.h>
#include <dory/core/services/sceneBuilder.h>
#include <spdlog/fmt/fmt.h>
#include <stack>

namespace dory::core::services
{
    using namespace resources;
    using namespace resources::scene;
    using namespace repositories;

    SceneBuilder::SceneBuilder(Registry& registry):
        DependencyResolver(registry)
    {}

    Scene* SceneBuilder::build(const scene::configuration::SceneConfiguration& configuration, DataContext& context)
    {
        auto sceneRepo = _registry.get<ISceneRepository, EcsType::entt>();
        auto sceneConfigRepo = _registry.get<ISceneConfigurationRepository>();
        auto sceneService = _registry.get<ISceneService, EcsType::entt>();
        auto logger = _registry.get<ILogService, Logger::App>();

        if(sceneRepo && sceneConfigRepo && sceneService)
        {
            const auto scene = sceneRepo->insert(Scene{ {}, configuration.name });
            if(!scene)
            {
                if(logger)
                {
                    logger->error(fmt::format("Cannot insert scene to SceneRepository"));
                }

                return nullptr;
            }

            auto configurationEntity = sceneConfigRepo->insert(configuration);
            if(!configurationEntity)
            {
                if(logger)
                {
                    logger->error(fmt::format("Cannot insert scene configuration to SceneConfigurationRepository"));
                }

                return nullptr;
            }

            scene->configurationId = configurationEntity->id;

            if(auto libraryService = _registry.get<ILibraryService>())
            {
                for(const auto& extension : configuration.extensions)
                {
                    libraryService->load(context, extension);
                }
            }

            for(const auto& [instance, type] : configurationEntity->devices)
            {
                if(auto device = instance.lock())
                {
                    device->connect(context);
                }
            }

            if(auto pipelineService = _registry.get<IPipelineService>())
            {
                pipelineService->buildPipeline(*scene, configurationEntity->pipeline, context);
            }

            for(const auto& [instance, type] : configurationEntity->directors)
            {
                if(auto director = instance.lock())
                {
                    director->initialize(*scene, *configurationEntity, context);
                }
            }

            return scene;
        }

        return nullptr;
    }

    void destroyScene(Scene& scene, DataContext& context, Registry& registry)
    {
        auto sceneConfigRepo = registry.get<ISceneConfigurationRepository>();
        auto sceneRepo = registry.get<ISceneRepository>(scene.ecsType);

        if(sceneConfigRepo && sceneRepo)
        {
            if(const auto sceneConfig = sceneConfigRepo->get(scene.configurationId))
            {
                //Call Directors do destroy the Scene in reverse order
                for(int i = static_cast<int>(sceneConfig->directors.size()) - 1; i >= 0; --i)
                {
                    const auto& [instance, type] = sceneConfig->directors[i];
                    if(auto director = instance.lock())
                    {
                        director->destroy(scene, *sceneConfig, context);
                    }
                }

                if(auto pipelineService = registry.get<IPipelineService>())
                {
                    pipelineService->destroyPipeline(scene, sceneConfig->pipeline, context);
                }

                for(const auto& [instance, type] : sceneConfig->devices)
                {
                    if(auto device = instance.lock())
                    {
                        device->disconnect(context);
                    }
                }

                if(auto libraryService = registry.get<ILibraryService>())
                {
                    for(const auto& extension : sceneConfig->extensions)
                    {
                        libraryService->unload(extension.name);
                    }
                }
            }

            sceneRepo->remove(scene.id);
        }
    }

    void SceneBuilder::destroy(Scene& scene, DataContext& context)
    {
        if(auto sceneRepo = _registry.get<ISceneRepository>(scene.ecsType))
        {
            std::stack<Scene*> hierarchy;
            std::stack<Scene*> stack;
            stack.push(&scene);

            //Build hierarchy stack
            while(!stack.empty())
            {
                auto currentScene = stack.top();
                stack.pop();

                hierarchy.push(currentScene);

                for(const auto childSceneId : currentScene->childScenes)
                {
                    if(const auto childScene = sceneRepo->get(childSceneId))
                    {
                        stack.push(childScene);
                    }
                }
            }

            //Traverse the tree from bottom up
            while(!hierarchy.empty())
            {
                const auto currentScene = hierarchy.top();
                hierarchy.pop();

                destroyScene(*currentScene, context, _registry);
            }
        }
    }
}
