#pragma once

#include <dory/core/services/iRenderer.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class OpenglRenderer: public IRenderer
    {
    private:
        Registry& _registry;

    public:
        explicit OpenglRenderer(Registry& registry);

        bool initialize(resources::DataContext& context) override;
        void draw(resources::DataContext& context, const resources::entity::Window& window, const resources::entity::View& view) override;
    };
}
