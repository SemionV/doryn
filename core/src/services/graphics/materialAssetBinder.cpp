#include <dory/core/registry.h>
#include <dory/core/services/graphics/materialAssetBinder.h>
#include <dory/core/resources/assets/material.h>
#include <dory/core/resources/assets/shader.h>

namespace dory::core::services::graphics
{
    MaterialAssetBinder::MaterialAssetBinder(Registry& registry): DependencyResolver(registry)
    {}

    void MaterialAssetBinder::bind(resources::IdType materialId, resources::entities::GraphicalContext& graphicalContext)
    {

    }
}