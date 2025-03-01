#pragma once

#include <dory/core/services/iLayoutService2.h>

namespace dory::core::services
{
    class LayoutService2: public ILayoutService2
    {
    public:
        resources::objects::layout::Container calculate(const resources::objects::layout::NodeSetupList& setupList,
            const resources::objects::layout::Variables& variables) final;
    };
}