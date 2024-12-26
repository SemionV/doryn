#pragma once

#include <vector>
#include <dory/generic/baseTypes.h>
#include <dory/core/resources/scene/scene.h>
#include <dory/core/resources/entities/view.h>

namespace dory::core::services
{
    class ISceneQueryService: public generic::Interface
    {
    public:
        virtual resources::scene::SceneState getSceneState(const resources::scene::Scene& scene) = 0;
    };
}