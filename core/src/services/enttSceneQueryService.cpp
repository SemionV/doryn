#include <dory/core/services/enttSceneQueryService.h>
#include <dory/core/resources/scene/enttScene.h>
#include <dory/core/resources/scene/components.h>
#include <dory/core/resources/scene/enttComponents.h>

namespace dory::core::services
{

    std::vector<resources::scene::VisibleObject> EnttSceneQueryService::getVisibleObjects(const resources::scene::Scene& scene)
    {
        return std::vector<resources::scene::VisibleObject>();
    }
}