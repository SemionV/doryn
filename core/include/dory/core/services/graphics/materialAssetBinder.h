#pragma once

#include <dory/core/services/iAssetBinder.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core::services::graphics
{
    class MaterialAssetBinder: public IAssetBinder, public DependencyResolver
    {
    public:
        explicit MaterialAssetBinder(Registry& registry);

        void bind(resources::IdType materialId, resources::entities::GraphicalContext& graphicalContext) override;
    };
}
