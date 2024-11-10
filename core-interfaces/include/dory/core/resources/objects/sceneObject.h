#pragma once

#include <dory/math/linearAlgebra.h>
#include "../id.h"
#include <string>

namespace dory::core::resources::objects
{
    struct SceneObject
    {
        std::string name;
        resources::IdType parentId;
        math::Vector3f position;
        math::Vector3f scale;
        IdType meshId;
    };
}
