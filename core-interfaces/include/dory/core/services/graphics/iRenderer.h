#pragma once

#include "dory/generic/baseTypes.h"
#include "dory/core/resources/dataContext.h"
#include "dory/core/resources/entities/window.h"
#include "dory/core/resources/entities/view.h"

namespace dory::core::services::graphics
{
    class IRenderer: public generic::Interface
    {
    public:
        virtual void draw(resources::DataContext& context, const resources::entities::View& view) = 0;
        virtual void draw(resources::DataContext& context,
                          const resources::entities::Window& window,
                          const resources::entities::GraphicalContext& graphicalContext,
                          const resources::entities::View& view) = 0;
    };
}
