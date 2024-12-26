#pragma once

#include <dory/core/dependencyResolver.h>
#include <dory/core/services/graphics/iRenderer.h>

namespace dory::core::services::graphics
{
    class Renderer: public IRenderer, public DependencyResolver
    {
    public:
        explicit Renderer(Registry& registry);

        virtual void draw(const resources::scene::SceneViewState& viewState,
                          float alpha,
                          const resources::entities::Window& window,
                          const resources::entities::GraphicalContext& graphicalContext) override;
    };
}