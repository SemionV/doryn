#pragma once

#include <dory/core/services/iSceneQueryService.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core::services
{
    class EnttSceneQueryService: public DependencyResolver, public ISceneQueryService
    {
    public:
        explicit EnttSceneQueryService(Registry& registry);

        resources::scene::SceneState getSceneState(const resources::entities::View& scene) override;
    };
}
