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

    Scene* SceneBuilder::build(const scene::configuration::Scene& configuration)
    {
        auto sceneRepo = _registry.get<ISceneRepository>();
        auto sceneService = _registry.get<ISceneService>();
        auto logger = _registry.get<ILogService>();

        if(sceneRepo && sceneService)
        {
            auto scene = sceneRepo->insert(Scene{ {}, configuration.name });
            if(!scene)
            {
                if(logger)
                {
                    logger->error(fmt::format("Cannot insert scene to SceneRepository"));
                }

                return nullptr;
            }

            return scene;
        }

        return nullptr;
    }
}
