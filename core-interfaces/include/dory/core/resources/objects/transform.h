#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace dory::core::resources::objects
{
    struct Transform
    {
        glm::vec3 position {};
        glm::quat rotation {};
        glm::vec3 scale {};
    };
}
