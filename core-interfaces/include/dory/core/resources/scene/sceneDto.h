#pragma once

#include <unordered_map>
#include <dory/core/resources/assets/shader.h>
#include <dory/core/resources/objects/transform.h>
#include <dory/core/resources/objects/materialProperties.h>

namespace dory::core::resources::scene::dto
{
    struct Shader
    {
        std::string filename {};
    };

    struct Material
    {
        std::unordered_map<assets::ShaderType, std::string> shaders {};
        objects::MaterialProperties properties;
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

    struct Object
    {
        objects::Transform transform {};
        std::string mesh {};
        std::string material {};
    };

    struct Scene
    {
        Assets assets;
        std::unordered_map<std::string, Object> objects {};
        std::unordered_map<std::string, std::string> cameras {};
        std::unordered_map<std::string, std::string> lights {};
    };
}