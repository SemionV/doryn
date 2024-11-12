#pragma once

#include <dory/core/resources/scene/scene.h>
#include <dory/core/resources/scene/object.h>

namespace dory::core::resources::scene
{
    struct DoryScene: public Scene
    {
        Object objects;
    };
}
