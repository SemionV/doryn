#pragma once

#include <dory/core/services/iSceneService.h>

namespace dory::core::services
{
    class SceneService: public ISceneService
    {
    public:
        void addObject(resources::scene::Scene& scene, const resources::objects::SceneObject& object) override;
    };
}
