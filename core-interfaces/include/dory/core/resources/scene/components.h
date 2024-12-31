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

    struct MovementAngularVelocity
    {
        glm::vec3 value{};
    };

    struct MovementLinearVelocity
    {
        float value{};
        glm::vec3 direction{};
    };

    struct MovementDistance
    {
        float left {};
        float done {};
        float accelerateUntil {};
        float decelerateFrom {};
    };

    struct MovementAcceleration
    {
        float speed {};
        float targetSpeed {};
    };
}
