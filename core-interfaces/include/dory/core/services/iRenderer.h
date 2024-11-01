#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/dataContext.h>
#include <dory/core/resources/entity.h>

namespace dory::core::services
{
    class IRenderer: public generic::Interface
    {
    public:
        virtual bool initialize(resources::DataContext& context) = 0;
        virtual void draw(resources::DataContext& context, const resources::entity::Window& window, const resources::entity::View& view) = 0;
    };
}
