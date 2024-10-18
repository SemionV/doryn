#pragma once

#include <dory/core/resources/dataContext.h>

namespace dory::core::devices
{
    class IDevice
    {
    public:
        virtual ~IDevice() = default;

        virtual void connect(resources::DataContext& context) = 0;
        virtual void disconnect(resources::DataContext& context) = 0;
    };
}
