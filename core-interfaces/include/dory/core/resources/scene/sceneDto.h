#pragma once

#include <unordered_map>
#include <vector>
#include <dory/core/resources/assets/shader.h>
#include <dory/core/resources/assets/material.h>
#include <dory/core/resources/objects/transform.h>
#include <dory/core/resources/objects/materialProperties.h>
#include <dory/core/resources/scene/components.h>

namespace dory::core::resources::scene::dto
{
    struct Shader
    {
        assets::ShaderType type {};
        std::string filename {};
    };

    struct Material
    {
        std::vector<std::string> baseMaterials {};
        std::vector<std::string> shaders {};
        objects::MaterialProperties properties {};
        assets::PolygonMode polygonMode {};
    };

    struct Mesh
    {
        std::string filename {};
    };

    struct Assets
    {
        std::unordered_map<std::string, Shader> shaders {};
        std::unordered_map<std::string, Material> materials {};
        std::unordered_map<std::string, Mesh> meshes {};
    };

    struct ObjectComponents
    {
        std::optional<components::LinearMovement> linearMovement {};
        std::optional<components::RotationMovement> rotationMovement {};
    };

    using vec2 = std::array<float, 2>;
    using vec3 = std::array<float, 3>;
    using vec4 = std::array<float, 4>;
    using quat = std::array<float, 4>;

    struct Transform
    {
        vec3 position;
        quat rotation;
        vec3 scale;
    };

    struct Object
    {
        Transform transform {};
        std::string mesh {};
        std::string material {};
        ObjectComponents components;
        std::unordered_map<std::string, Object> children {};
    };

    struct PhysicsController
    {
        float precision {};
    };

    struct AnimationController
    {
        std::size_t frames {};
    };

    struct ViewController
    {};

    struct Controller
    {
        std::optional<PhysicsController> physics {};
        std::optional<AnimationController> animation {};
        std::optional<ViewController> view {};
    };

    struct ElapsedTimeTrigger
    {
        std::optional<float> seconds {};
        std::optional<float> milliseconds {};
        std::optional<float> microseconds {};
        std::optional<float> nanoseconds {};
    };

    struct LogicTrigger
    {
        bool flag {};
    };

    struct Trigger
    {
        std::optional<ElapsedTimeTrigger> elapsedTime {};
        std::optional<LogicTrigger> logic {};
    };

    struct Node
    {
        std::string node {};
        std::string parent {};
        Controller controller {};
        Trigger trigger {};
        std::vector<Node> children {};
    };

    struct Scene
    {
        Assets assets;
        std::unordered_map<std::string, Object> objects {};
        std::unordered_map<std::string, std::string> cameras {};
        std::unordered_map<std::string, std::string> lights {};
        std::vector<Node> pipeline {};
    };
}