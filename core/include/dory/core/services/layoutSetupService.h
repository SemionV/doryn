#pragma once

#include <dory/core/services/iLayoutSetupService.h>

namespace dory::core::services
{
    class LayoutSetupService: public ILayoutSetupService
    {
    public:
        resources::objects::layout::NodeSetupList buildSetupList(const resources::objects::layout::ContainerDefinition& containerDefinition) final;
    };
}