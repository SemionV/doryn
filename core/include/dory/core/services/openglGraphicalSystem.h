#pragma once

#include <dory/core/services/iGraphicalSystem.h>
#include <dory/core/resources/entities/window.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class OpenglGraphicalSystem: public IGraphicalSystem
    {
    private:
        Registry& _registry;

    public:
        explicit OpenglGraphicalSystem(Registry& registry);

        bool initializeGraphics(const resources::entities::Window& window) override;
        void setCurrentWindow(const resources::entities::Window& window) override;
        void swapBuffers(const resources::entities::Window& window) override;
    };
}
