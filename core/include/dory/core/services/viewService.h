#pragma once

#include <dory/core/dependencyResolver.h>
#include <dory/core/services/iViewService.h>
#include <dory/core/resources/dataContext.h>

namespace dory::core::services
{
    class ViewService: public IViewService, public DependencyResolver
    {
    public:
        explicit ViewService(Registry& registry);

        void updateViews(resources::DataContext& context) override;
    };
}
