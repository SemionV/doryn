#pragma once

#include <dory/core/dependencyResolver.h>

namespace dory::core
{
    class Registry;
}

namespace dory::game::logic
{
    enum class MoveDirection
    {
        up,
        down,
        left,
        right
    };

    class CameraService: public core::DependencyResolver
    {
    public:
        explicit CameraService(core::Registry& registry);

        void moveCamera(core::resources::DataContext& dataContext, MoveDirection direction, const core::resources::entities::View& view);
        void stopCamera(core::resources::DataContext& dataContext, const core::resources::entities::View& view);
    };
}
