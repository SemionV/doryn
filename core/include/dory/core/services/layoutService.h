#pragma once

#include <dory/core/services/iLayoutService.h>

namespace dory::core::services
{
    class LayoutService: public ILayoutService
    {
    public:
        resources::objects::layout::DetachedContainer calculate(const resources::scene::configuration::layout::PositionedContainer& layoutDefinition,
            const resources::objects::layout::Size& availableSpace) final;
    };
}