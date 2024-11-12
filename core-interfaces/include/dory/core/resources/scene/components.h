#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace dory::core::resources::scene::components
{
    struct Name
    {
        std::string name{};
    };

    struct Position
    {
        glm::vec3 value;
    };

    struct Scale
    {
        glm::vec3 value;
    };

    struct Mesh
    {
        IdType id{};
    };
}
