#pragma once

#include <vector>
#include <dory/generic/baseTypes.h>
#include <dory/core/resources/scene/scene.h>

namespace dory::core::services
{
    class ISceneQueryService: public generic::Interface
    {
    public:
        virtual std::vector<resources::scene::VisibleObject> getVisibleObjects(const resources::scene::Scene& scene) = 0;
    };
}

