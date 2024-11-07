#pragma once

#include "../entity.h"
#include <memory>

namespace dory::core::resources::entities
{
    class IWindowSubsystemData
    {
    public:
        virtual ~IWindowSubsystemData() = default;
    };

    struct Window: public Entity<IdType>
    {
        Window() = default;

        explicit Window(IdType id, WindowSystem windowSystem, GraphicalSystem graphicalSystem):
                Entity(id),
                windowSystem(windowSystem),
                graphicalSystem(graphicalSystem)
        {}

        WindowSystem windowSystem {};
        GraphicalSystem graphicalSystem {};
        std::shared_ptr<IWindowSubsystemData> windowSystemData;
    };
}
