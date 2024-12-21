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
        auto view = enttScene.registry.view<Mesh, CombinedTransform>();

        for (auto entity : view)
        {
            auto& mesh = view.get<Mesh>(entity);
            auto& transform = view.get<CombinedTransform>(entity);

            objects.emplace_back(VisibleObject { mesh.id, transform.matrix });
        }

        return objects;
    }
}