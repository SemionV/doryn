#pragma once

#include "../entity.h"
#include <dory/core/resources/systemTypes.h>

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