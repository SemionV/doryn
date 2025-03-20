#pragma once

#include <dory/core/services/iLayoutService.h>

namespace dory::core::services
{
    class LayoutService: public ILayoutService
    {
    public:
        void buildLayout(const resources::objects::layout::NodeSetupList& setupList,
            const resources::objects::layout::Variables& variables, resources::entities::layout::Layout& layout) final;
    };
}