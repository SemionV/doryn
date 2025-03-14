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

        explicit Window(IdType id, WindowSystem windowSystem, IdType graphicalContextId):
                Entity(id),
                windowSystem(windowSystem),
                graphicalContextId(graphicalContextId)
        {}

        WindowSystem windowSystem {};
        resources::IdType graphicalContextId {};
        unsigned int width {};
        unsigned int height {};
        std::vector<IdType> views;
    };
}