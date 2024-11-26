#pragma once

#include <dory/core/dependencyResolver.h>
#include <dory/core/services/graphics/iRenderer.h>

namespace dory::core::services::graphics
{
    class Renderer: public IRenderer, public DependencyResolver
    {
    public:
        explicit Renderer(Registry& registry);

        void draw(resources::DataContext& context, const resources::entities::View& view) override;

        virtual void draw(resources::DataContext& context,
                          const resources::entities::Window& window,
                          const resources::entities::GraphicalContext& graphicalContext,
                          const resources::entities::View& view) override;
    };
}