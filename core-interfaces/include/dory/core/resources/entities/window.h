#pragma once

#include "../entity.h"
#include <memory>
#include <dory/core/resources/windowSystem.h>
#include <dory/core/resources/graphicalSystem.h>

namespace dory::core::resources::entities
{
    struct Window: public Entity<IdType>
    {
        Window() = default;

        explicit Window(IdType id, WindowSystem windowSystem, GraphicalSystem graphicalSystem):
                Entity(id),
                windowSystem(windowSystem),
                graphicalSystem(graphicalSystem)
        {}

        GraphicalSystem graphicalSystem {};
        WindowSystem windowSystem {};
        resources::IdType graphicalContextBindingsId {};
    };
}
