#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/scene/configuration.h>
#include <dory/core/resources/objects/layout.h>

namespace dory::core::services
{
    class ILayoutSetupService: public generic::Interface
    {
    public:
        virtual resources::objects::layout::NodeSetupList buildSetupList(const resources::scene::configuration::layout2::ContainerDefinition& containerDefinition) = 0;
    };
}
