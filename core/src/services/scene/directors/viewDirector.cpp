#include <dory/core/registry.h>
#include <dory/core/services/scene/directors/viewDirector.h>

namespace dory::core::services::scene::directors
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;

    ViewDirector::ViewDirector(Registry& registry):
        DependencyResolver(registry)
    {}

    void ViewDirector::initialize(Scene& scene, SceneConfiguration& sceneConfig, DataContext& context)
    {
    }

    void ViewDirector::destroy(Scene& scene, SceneConfiguration& sceneConfig, DataContext& context)
    {
    }
}