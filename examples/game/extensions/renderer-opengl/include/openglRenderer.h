#pragma once

#include <dory/core/registry.h>

namespace dory::renderer::opengl
{
    class OpenglRenderer: public core::services::IRenderer
    {
    private:
        core::Registry& _registry;

    public:
        explicit OpenglRenderer(core::Registry& registry);

        bool initialize(core::resources::DataContext& context) override;
        void draw(core::resources::DataContext& context, const core::resources::entity::Window& window, const core::resources::entity::View& view) override;
    };
}
