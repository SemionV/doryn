#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/entities/graphicalContext.h>

namespace dory::core::services::graphics
{
    class IAssetBinder: public generic::Interface
    {
    public:
        virtual void bindMesh(resources::IdType meshId, resources::entities::GraphicalContext& graphicalContext) = 0;
    };
}
