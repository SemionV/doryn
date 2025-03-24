#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/entities/display.h>

namespace dory::core::services
{
    class IDisplayService: public generic::Interface
    {
    public:
        virtual bool initializeDisplays() = 0;
    };
}