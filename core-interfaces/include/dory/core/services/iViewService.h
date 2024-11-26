#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/entities/window.h>
#include <dory/core/resources/dataContext.h>

namespace dory::core::services
{
    class IViewService: public generic::Interface
    {
    public:
        virtual void updateViews(resources::DataContext& context) = 0;
    };
}
