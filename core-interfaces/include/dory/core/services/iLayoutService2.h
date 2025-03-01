#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/scene/configuration.h>
#include <dory/core/resources/objects/layout.h>

namespace dory::core::services
{
    class ILayoutService2: public generic::Interface
    {
    public:
        virtual resources::objects::layout::Container calculate(const resources::objects::layout::NodeSetupList& setupList,
            const resources::objects::layout::Variables& variables) = 0;
    };
}
