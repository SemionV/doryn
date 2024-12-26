#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/entities/window.h>
#include <dory/core/resources/scene/scene.h>
#include <dory/core/resources/dataContext.h>

namespace dory::core::services
{
    class IViewService: public generic::Interface
    {
    public:
        virtual void updateViews(resources::scene::SceneViewStateSet& viewStates, float alpha) = 0;
        virtual resources::entities::View* createView(resources::entities::Window& window) = 0;
        virtual void destroyView(resources::IdType viewId) = 0;
        virtual void updateViewsState(resources::scene::SceneViewStateSet& states) = 0;
    };
}
