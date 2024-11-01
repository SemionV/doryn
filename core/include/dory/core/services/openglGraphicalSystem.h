#pragma once

#include <dory/core/services/iGraphicalSystem.h>

namespace dory::core::services
{
    class OpenglGraphicalSystem: public IGraphicalSystem
    {
    public:
        bool initializeGraphics(const resources::entity::Window& window) override;
        void setCurrentWindow(const resources::entity::Window& window) override;
        void swapBuffers(const resources::entity::Window& window) override;
    };
}
