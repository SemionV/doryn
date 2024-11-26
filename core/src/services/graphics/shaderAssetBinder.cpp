#include <dory/core/registry.h>
#include <dory/core/services/graphics/shaderAssetBinder.h>
#include <dory/core/resources/assets/shader.h>

namespace dory::core::services::graphics
{
    ShaderAssetBinder::ShaderAssetBinder(Registry& registry) : DependencyResolver(registry)
    {}

    void ShaderAssetBinder::bind(resources::IdType shaderId, resources::entities::GraphicalContext& graphicalContext)
    {
    }
}