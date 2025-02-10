#include <dory/core/registry.h>
#include <dory/core/services/sceneBuilder.h>
#include <spdlog/fmt/fmt.h>

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
        auto sceneRepo = _registry.get<ISceneRepository>();
        auto sceneConfigRepo = _registry.get<ISceneConfigurationRepository>();
        auto sceneService = _registry.get<ISceneService>();
        auto logger = _registry.get<ILogService>();

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

    void SceneBuilder::destroy(const Scene& scene, DataContext& context)
    {
        //TODO: implement
    }
}
