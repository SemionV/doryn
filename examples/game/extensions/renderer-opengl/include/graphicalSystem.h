#pragma once

#include <dory/core/services/iGraphicalSystem.h>

namespace dory::core
{
    class Registry;
}

namespace dory::renderer::opengl
{
    class GraphicalSystem: public core::services::IGraphicalSystem
    {
    private:
        core::Registry& _registry;

    public:
        explicit GraphicalSystem(core::Registry& registry);

        void render(core::resources::DataContext& context, const core::resources::entities::Window& window) override;
    };
}