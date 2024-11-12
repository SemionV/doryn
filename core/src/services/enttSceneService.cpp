#include <dory/core/services/enttSceneService.h>
#include <dory/core/resources/scene/enttScene.h>

namespace dory::core::services
{
    resources::IdType EnttSceneService::addObject(resources::scene::Scene& scene, const resources::objects::SceneObject& object)
    {
        return 0;
    }

    void EnttSceneService::deleteObject(resources::scene::Scene& scene, resources::IdType objectId)
    {
    }
}