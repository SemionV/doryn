#pragma once

#include <dory/core/services/iSceneQueryService.h>
#include <entt/entt.hpp>

namespace dory::core::services
{
    class EnttSceneQueryService: public ISceneQueryService
    {
    public:
        std::vector<resources::scene::VisibleObject> getVisibleObjects(const resources::scene::Scene& scene) override;
    };
}
