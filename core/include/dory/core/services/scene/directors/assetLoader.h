#pragma once

#include <dory/core/services/iSceneDirector.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core::services::scene::directors
{
    class AssetLoader: public ISceneDirector, public DependencyResolver
    {
    public:
        explicit AssetLoader(Registry& registry);

        void initialize(resources::scene::Scene& scene, resources::scene::configuration::SceneConfiguration& sceneConfig, resources::DataContext& context) final;
        void destroy(resources::scene::Scene& scene, resources::scene::configuration::SceneConfiguration& sceneConfig, resources::DataContext& context) final;
    };
}
