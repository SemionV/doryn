#pragma once

#include <dory/core/services/iSceneBuilder.h>
#include <dory/core/dependencyResolver.h>

namespace dory::game
{
    class DemoSceneBuilder: public core::services::ISceneBuilder, public core::DependencyResolver
    {
    public:
        explicit DemoSceneBuilder(core::Registry& registry);

        core::resources::scene::Scene* build(core::resources::DataContext& context) final;
    };
}
