#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/scene/configuration.h>
#include <dory/core/resources/objects/layout.h>

namespace dory::core::services
{
    class ILayoutService: public generic::Interface
    {
    public:
        virtual resources::objects::layout::Container calculate(const resources::scene::configuration::layout::ContainerDefinition& layoutDefinition,
            const resources::objects::layout::Size& availableSpace) = 0;
    };
}
