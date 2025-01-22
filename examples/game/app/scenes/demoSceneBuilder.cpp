#include "demoSceneBuilder.h"

namespace dory::game
{
    DemoSceneBuilder::DemoSceneBuilder(core::Registry& registry): core::DependencyResolver(registry)
    {}

    core::resources::scene::Scene* DemoSceneBuilder::build(core::resources::DataContext& context)
    {
    }
}
