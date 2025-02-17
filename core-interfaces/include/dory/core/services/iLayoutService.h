#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/scene/configuration.h>

namespace dory::core::services
{
    class ILayoutService: public generic::Interface
    {
    public:
        virtual resources::scene::configuration::layout::Container calculate(const resources::scene::configuration::layout::Container& container,
            const resources::scene::configuration::layout::Size& constraintSize) = 0;
    };
}
