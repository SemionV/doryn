#pragma once

#include <dory/core/services/iLayoutService.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core::services
{
    class LayoutService: public ILayoutService
    {
    public:
        std::unique_ptr<resources::entities::layout::Container> calculate(const resources::objects::layout::NodeSetupList& setupList,
            const resources::objects::layout::Variables& variables) final;

        void buildLayout(const resources::objects::layout::NodeSetupList& setupList,
            const resources::objects::layout::Variables& variables, resources::entities::layout::Layout& layout) final;
    };
}