#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/objects/sceneObject.h>
#include <dory/core/resources/id.h>
#include <dory/core/resources/scene/scene.h>

namespace dory::core::services
{
    class ISceneService: public generic::Interface
    {
    public:
        virtual void addObject(resources::scene::Scene& scene, const resources::objects::SceneObject& object) = 0;
    };
}
