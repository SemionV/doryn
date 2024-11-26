#pragma once

#include <dory/core/services/iAssetBinder.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core::services::graphics
{
    class ShaderAssetBinder: public IAssetBinder, public DependencyResolver
    {
    public:
        explicit ShaderAssetBinder(Registry& registry);

        void bind(resources::IdType shaderId, resources::entities::GraphicalContext& graphicalContext) override;
    };
}