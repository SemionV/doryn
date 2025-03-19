#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/scene/configuration.h>
#include <dory/core/resources/objects/layout.h>

namespace dory::core::services
{
    class ILayoutService: public generic::Interface
    {
    public:
        virtual std::unique_ptr<resources::objects::layout::Container> calculate(const resources::objects::layout::NodeSetupList& setupList,
            const resources::objects::layout::Variables& variables) = 0;
    };
}
