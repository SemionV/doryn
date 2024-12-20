#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace dory::core::resources::objects
{
    struct Transform
    {
        glm::vec3 position {0, 0, 0};
        glm::quat rotation {};
        glm::vec3 scale {1, 1, 1};
    };
}
