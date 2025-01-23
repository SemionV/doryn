#pragma once

#include "../id.h"
#include "transform.h"
#include <string>

namespace dory::core::resources::objects
{
    struct SceneObject
    {
        std::string name {};
        resources::IdType parentId {};
        Transform transform {};
    };
}
