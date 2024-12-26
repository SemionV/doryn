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

    struct CombinedTransform: public Transform
    {
        CombinedTransform() = default;

        explicit CombinedTransform(const objects::Transform& transform): Transform(transform)
        {}
    };

    struct Rotation
    {
        float angleSpeed{};
        glm::vec3 axis{};
        float currentAngle{};
    };

    struct Translation
    {
        float speed;
        glm::vec3 direction{};
        glm::vec3 currentPosition{};
    };
}
