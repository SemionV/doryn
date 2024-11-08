#pragma once

#include <dory/core/services/iGraphicalSystem.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class GraphicalSystem: public core::services::IGraphicalSystem
    {
    private:
        core::Registry& _registry;

    public:
        explicit GraphicalSystem(core::Registry& registry);

        bool uploadProgram(const core::resources::entities::ShaderProgram& program, const core::resources::entities::Window& window) override;
        void render(core::resources::DataContext& context, const core::resources::entities::Window& window) override;
    };
}