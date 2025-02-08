#pragma once

#include <dory/core/services/iSceneBuilder.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class SceneBuilder: public ISceneBuilder, public DependencyResolver
    {
    public:
        explicit SceneBuilder(Registry& registry);

        resources::scene::Scene* build(const resources::scene::configuration::Scene& configuration) final;
    };
}