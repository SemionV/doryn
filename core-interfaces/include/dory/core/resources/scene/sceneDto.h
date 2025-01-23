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
        objects::MaterialProperties properties;
        assets::PolygonMode polygonMode;
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
        std::optional<components::LinearMovement> linearMovement;
        std::optional<components::RotationMovement> rotationMovement;
    };

    struct Object
    {
        objects::Transform transform {};
        std::string mesh {};
        std::string material {};
        ObjectComponents components;
        std::unordered_map<std::string, Object> children {};
    };

    struct Scene
    {
        Assets assets;
        std::unordered_map<std::string, Object> objects {};
        std::unordered_map<std::string, std::string> cameras {};
        std::unordered_map<std::string, std::string> lights {};
    };
}