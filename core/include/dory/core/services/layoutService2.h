#pragma once

#include <dory/core/services/iLayoutService2.h>

namespace dory::core::services
{
    class LayoutService2: public ILayoutService2
    {
    public:
        resources::objects::layout::Container calculate(const resources::scene::configuration::layout::ContainerDefinition& layoutDefinition) final;
    };
}