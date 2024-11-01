#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/windowParameters.h>
#include <dory/core/resources/entity.h>
#include <dory/core/resources/id.h>

namespace dory::core::services
{
    class IWindowService: public generic::Interface
    {
    public:
        virtual resources::IdType createWindow(const resources::WindowParameters& parameters) = 0;
        virtual void closeWindow(resources::IdType windowId) = 0;
    };
}
