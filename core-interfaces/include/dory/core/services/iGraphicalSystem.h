#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/entity.h>
#include <dory/core/resources/entities/window.h>

namespace dory::core::services
{
    class IGraphicalSystem: generic::Interface
    {
    public:
        virtual bool initializeGraphics(const resources::entities::Window& window) = 0;
        virtual void setCurrentWindow(const resources::entities::Window& window) = 0;
        virtual void swapBuffers(const resources::entities::Window& window) = 0;
    };
}
