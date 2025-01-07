#pragma once

#include <dory/core/dependencyResolver.h>
#include <dory/core/services/iViewService.h>
#include <dory/core/resources/dataContext.h>
#include <dory/core/resources/entities/view.h>

namespace dory::core::services
{
    class ViewService: public IViewService, public DependencyResolver
    {
    public:
        explicit ViewService(Registry& registry);

        void updateViews(resources::scene::SceneViewStateSet& viewStates, float alpha, resources::DataContext& dataContext) override;
        resources::entities::View* createView(resources::entities::Window& window) override;
        void destroyView(resources::IdType viewId) override;
        void updateViewsState(resources::scene::SceneViewStateSet& states) override;
    };
}
