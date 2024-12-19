#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/windowParameters.h>
#include <dory/core/resources/entity.h>
#include <dory/core/resources/id.h>
#include <dory/core/resources/entities/window.h>

namespace dory::core::services
{
    class IWindowService: public generic::Interface
    {
    public:
        virtual resources::entities::Window* createWindow(const resources::WindowParameters& parameters, resources::WindowSystem windowSystem) = 0;
        virtual void closeWindow(resources::IdType windowId, resources::WindowSystem windowSystem) = 0;
        virtual void setCurrentWindow(const resources::entities::Window& window) = 0;
        virtual void swapBuffers(const resources::entities::Window& window) = 0;
    };
}
