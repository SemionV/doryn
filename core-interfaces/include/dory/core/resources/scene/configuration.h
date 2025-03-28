#pragma once

#include <unordered_map>
#include <vector>
#include <dory/core/resources/configuration.h>
#include <dory/core/resources/assets/shader.h>
#include <dory/core/resources/assets/material.h>
#include <dory/core/resources/scene/components.h>
#include <dory/core/resources/factory.h>
#include <dory/core/resources/systemTypes.h>
#include <dory/core/iController.h>
#include <dory/core/devices/iDevice.h>
#include <dory/core/services/iSceneDirector.h>

namespace dory::core::resources::scene::configuration
{
    struct Shader
    {
        assets::ShaderType type {};
        std::string filename {};
    };

    struct MaterialProperties
    {
        std::optional<glm::vec4> color {};
    };

    struct Material
    {
        std::vector<std::string> baseMaterials {};
        std::vector<std::string> shaders {};
        MaterialProperties properties {};
        std::optional<assets::PolygonMode> polygonMode {};
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

    struct Transform
    {
        glm::vec3 position {};
        glm::quat rotation {};
        glm::vec3 scale {};
    };

    struct Object
    {
        Transform transform {};
        std::string mesh {};
        std::string material {};
        ObjectComponents components;
        std::unordered_map<std::string, Object> children {};
    };

    struct Node
    {
        IdType pipelineNodeId {};
        std::string parent {};
        factory::Instance<ITrigger> trigger {};
        factory::Instance<IController> controllerInstance {};
        std::string controller {};
        std::unordered_map<std::string, Node> children {};
    };

    struct Pipeline
    {
        std::unordered_map<std::string, Node> nodes {};
    };

    struct GraphicalContext
    {
        GraphicalSystem graphicalSystem {};
    };

    struct Window
    {
        std::string title {};
        std::string graphicalContext {};
    };

    struct View
    {
        std::string container; //attach to a grid column
    };

    struct SceneConfiguration: Entity<>
    {
        std::string name;
        Assets assets;
        std::unordered_map<std::string, Object> objects {};
        std::unordered_map<std::string, std::string> cameras {};
        std::unordered_map<std::string, std::string> lights {};
        Pipeline pipeline;
        std::vector<resources::configuration::Extension> extensions;
        std::vector<factory::Instance<devices::IDevice>> devices;
        std::vector<factory::Instance<services::ISceneDirector>> directors;
        std::unordered_map<std::string, GraphicalContext> graphicalContexts {};
        std::unordered_map<std::string, Window> windows {};
    };
}