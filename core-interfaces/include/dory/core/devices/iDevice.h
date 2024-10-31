#pragma once

#include <dory/core/resources/dataContext.h>
#include <dory/generic/baseTypes.h>

namespace dory::core::devices
{
    class IDevice: public generic::Interface
    {
    public:
        virtual void connect(resources::DataContext& context) = 0;
        virtual void disconnect(resources::DataContext& context) = 0;
    };
}
