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

    struct Object
    {
        IdType id;
    };

    struct Mesh
    {
        IdType id{};
    };

    struct Material
    {
        IdType id{};
    };

    struct Scale
    {
        glm::vec3 value{};
    };

    struct Orientation
    {
        glm::quat value{};
    };

    struct Position
    {
        glm::vec3 value{};
    };

    struct WorldTransform
    {
        glm::vec3 scale {};
        glm::quat orientation {};
        glm::vec3 position {};

        WorldTransform() = default;

        explicit WorldTransform(const objects::Transform& transform):
                position(transform.position),
                orientation(transform.rotation),
                scale(transform.scale)
        {}
    };

    struct AngularVelocity
    {
        glm::vec3 value{};
    };

    struct LinearVelocity
    {
        glm::vec3 initialValue{};
        glm::vec3 value{};

        LinearVelocity() = default;

        explicit LinearVelocity(glm::vec3 value): initialValue(value), value(value)
        {}
    };

    struct Distance
    {
        float left {};
        float done {};
    };

    struct Deceleration
    {
        float speed {};
        float threshold {};
    };
}
