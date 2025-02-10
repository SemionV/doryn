#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/scene/scene.h>

namespace dory::core::resources::scene::configuration
{
    struct SceneConfiguration;
}

namespace dory::core::services
{
    class ISceneDirector: generic::Interface
    {
    public:
        virtual void initialize(resources::scene::Scene& scene, resources::scene::configuration::SceneConfiguration& sceneConfig, resources::DataContext& context) = 0;
        virtual void destroy(resources::scene::Scene& scene, resources::DataContext& context) = 0;
    };
}
