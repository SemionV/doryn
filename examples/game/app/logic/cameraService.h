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

        void moveCamera(MoveDirection direction, const core::resources::entities::View& view);
    };
}
