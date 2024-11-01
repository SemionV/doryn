#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/entity.h>

namespace dory::core::services
{
    class IGraphicalSystem: generic::Interface
    {
    public:
        virtual bool initializeGraphics(const resources::entity::Window& window) = 0;
    };
}
