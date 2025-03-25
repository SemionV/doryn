#pragma once

#include <string_view>

namespace dory::core::resources
{
    enum class WindowSystem
    {
        glfw,
        win32,
        x,
        wayland
    };

    enum class DisplaySystem
    {
        glfw
    };

    enum class GraphicalSystem
    {
        opengl,
        vulkan,
        directx
    };

    enum class EcsType
    {
        entt,
        dory
    };

    struct AssetTypeName
    {
        constexpr static std::string_view extension = "extension";
        constexpr static std::string_view mesh = "mesh";
        constexpr static std::string_view material = "material";
        constexpr static std::string_view shader = "shader";
    };

    enum class AssetFileFormat
    {
        bmp,
        png
    };
}
