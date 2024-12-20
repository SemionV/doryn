#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <string>

namespace dory::core::resources::scene::components
{
    struct Name
    {
        std::string name{};
    };

    struct Mesh
    {
        IdType id{};
    };

    struct Transform
    {
        glm::vec3 position {};
        glm::quat rotation {};
        glm::vec3 scale {};

        Transform() = default;

        explicit Transform(const objects::Transform& transform):
                position(transform.position),
                rotation(transform.rotation),
                scale(transform.scale)
        {}
    };

    struct LocalTransform: public Transform
    {
        LocalTransform() = default;

        explicit LocalTransform(const objects::Transform& transform): Transform(transform)
        {}
    };

    struct WorldTransform: public Transform
    {
        WorldTransform() = default;

        explicit WorldTransform(const objects::Transform& transform): Transform(transform)
        {}
    };

}
