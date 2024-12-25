#pragma once

#include "dory/generic/baseTypes.h"
#include "dory/core/resources/dataContext.h"
#include "dory/core/resources/scene/scene.h"
#include "dory/core/resources/entities/window.h"
#include "dory/core/resources/entities/view.h"
#include "dory/core/resources/entities/graphicalContext.h"

namespace dory::core::services::graphics
{
    class IRenderer: public generic::Interface
    {
    public:
        virtual void draw(const resources::scene::SceneViewState& viewState,
                          float alpha,
                          const resources::entities::Window& window,
                          const resources::entities::GraphicalContext& graphicalContext) = 0;
    };
}
