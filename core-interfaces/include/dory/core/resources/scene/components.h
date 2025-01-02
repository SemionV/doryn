#pragma once

#include <dory/core/resources/objects/transform.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
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

    struct AccelerationMovement
    {
        glm::vec3 value;
        float distanceDone;
        float highVelocity {};
        float lowVelocity {};
        float currentVelocity {};
        float decelerationDistance {};
        float acceleration {};
        float deceleration {};
        float step {};
    };

    struct RotationMovement: public AccelerationMovement
    {};

    struct LinearMovement: public AccelerationMovement
    {};

}
