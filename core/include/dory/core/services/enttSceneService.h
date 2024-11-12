#pragma once

#include <dory/core/services/iSceneService.h>

namespace dory::core::services
{
    class EnttSceneService: public ISceneService
    {
    public:
        resources::IdType addObject(resources::scene::Scene& scene, const resources::objects::SceneObject& object) override;
        void deleteObject(resources::scene::Scene& scene, resources::IdType objectId) override;
    };
}
