#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/objects/layout.h>
#include <dory/core/resources/scene/configuration.h>
#include <dory/core/resources/entities/layout.h>

namespace dory::core::services
{
    class ILayoutService: public generic::Interface
    {
    public:
        virtual void buildLayout(const resources::objects::layout::NodeSetupList& setupList,
            const resources::objects::layout::Variables& variables, resources::entities::layout::Layout& layout) = 0;
    };
}
