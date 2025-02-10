#include <dory/core/registry.h>
#include <dory/core/services/scene/directors/assetLoader.h>

namespace dory::core::services::scene::directors
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;

    AssetLoader::AssetLoader(Registry& registry):
        DependencyResolver(registry)
    {}

    void AssetLoader::initialize(Scene& scene, SceneConfiguration& sceneConfig, DataContext& context)
    {

    }

    void AssetLoader::destroy(Scene& scene, SceneConfiguration& sceneConfig, DataContext& context)
    {
    }
}
