#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/scene/scene.h>

namespace dory::core::services
{
    class IGraphicContextService: public generic::Interface
    {
    public:
        virtual void bindMesh(resources::IdType sceneId, resources::IdType meshId) = 0;
    };
}
