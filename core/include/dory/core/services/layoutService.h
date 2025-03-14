#pragma once

#include <dory/core/services/iLayoutService.h>

namespace dory::core::services
{
    class LayoutService: public ILayoutService
    {
    public:
        resources::objects::layout::Container calculate(const resources::scene::configuration::layout::ContainerDefinition& layoutDefinition,
            const resources::objects::layout::Size& availableSpace) final;
    };
}