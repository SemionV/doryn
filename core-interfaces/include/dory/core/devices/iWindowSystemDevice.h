#pragma once

#include "iDevice.h"
#include <dory/core/resources/dataContext.h>
#include <dory/core/resources/entities/window.h>
#include <dory/core/resources/entities/display.h>
#include <dory/core/resources/windowParameters.h>

namespace dory::core::devices
{
    class IWindowSystemDevice: public IDevice
    {
    public:
        virtual void pollEvents(resources::DataContext& context) = 0;
        virtual void setupWindow(resources::entities::Window& window, const resources::WindowParameters& parameters) = 0;
        virtual void closeWindow(const resources::entities::Window& window) = 0;
        virtual void setCurrentWindow(const resources::entities::Window& window) = 0;
        virtual void swapWindowBuffers(const resources::entities::Window& window) = 0;
    };
}
