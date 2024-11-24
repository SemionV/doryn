#pragma once

#include <dory/core/services/graphics/iAssetBinder.h>
#include <dory/core/resources/entities/graphicalContext.h>
#include "dory/core/resources/assets/mesh.h"

namespace dory::core
{
    class Registry;
}

namespace dory::core::services::graphics
{
    class AssetBinder: public IAssetBinder
    {
    private:
        Registry& _registry;


        void bindMeshData(resources::entities::GraphicalContext& graphicalContext, const resources::assets::Mesh* mesh, resources::bindings::MeshBinding* meshBinding);

    public:
        explicit AssetBinder(Registry& registry);

        void bindMesh(resources::IdType meshId, resources::entities::GraphicalContext& graphicalContext) override;
    };
}

