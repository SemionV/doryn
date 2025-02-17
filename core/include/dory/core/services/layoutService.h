#pragma once

#include <dory/core/services/iLayoutService.h>

namespace dory::core::services
{
    class LayoutService: public ILayoutService
    {
    public:
        resources::scene::configuration::layout::Container calculate(const resources::scene::configuration::layout::Container& container,
            const resources::scene::configuration::layout::Size& constraintSize) final;
    };
}