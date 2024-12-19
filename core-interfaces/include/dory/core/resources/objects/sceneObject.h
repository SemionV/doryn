#pragma once

#include <dory/math/linearAlgebra.h>
#include "../id.h"
#include "transform.h"
#include <string>

namespace dory::core::resources::objects
{
    struct SceneObject
    {
        std::string name;
        resources::IdType parentId;
        IdType meshId;

        Transform localTransform;
        Transform worldTransform;
    };
}
