#pragma once

#include <dory/core/registry.h>

namespace dory::renderer::opengl
{
    class OpenglRenderer: public core::services::graphics::IRenderer
    {
    private:
        core::Registry& _registry;

    public:
        explicit OpenglRenderer(core::Registry& registry);

        void draw(core::resources::DataContext& context, const core::resources::entities::View& view) override;
        void draw(core::resources::DataContext& context,
                  const core::resources::entities::Window& window,
                  const core::resources::entities::GraphicalContext& graphicalContext,
                  const core::resources::entities::View& view) override;
    };
}
