#include <dory/core/services/enttSceneQueryService.h>
#include <dory/core/resources/scene/enttScene.h>
#include <dory/core/resources/scene/components.h>

namespace dory::core::services
{
    using namespace resources::scene;
    using namespace resources::scene::components;

    std::vector<resources::scene::VisibleObject> EnttSceneQueryService::getVisibleObjects(const resources::scene::Scene& scene)
    {
        auto objects = std::vector<VisibleObject>();
        const auto& enttScene = (const EnttScene&)scene;
        auto view = enttScene.registry.view<Mesh, LocalTransform, WorldTransform>();

        for (auto entity : view)
        {
            auto &mesh = view.get<Mesh>(entity);
            auto &localTransform = view.get<LocalTransform>(entity);
            auto &worldTransform = view.get<WorldTransform>(entity);

            objects.emplace_back(VisibleObject { localTransform, worldTransform, mesh.id });
        }

        return objects;
    }
}