#pragma once

#include <string_view>
#include <dory/core/resources/name.h>
#include <dory/containers/hashId.h>

namespace dory::core::resources
{
    using containers::hash::operator""_id;

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
        constexpr static Name extension = "extension"_id;
        constexpr static Name mesh = "mesh"_id;
        constexpr static Name material = "material"_id;
        constexpr static Name shader = "shader"_id;
    };

    enum class AssetFileFormat
    {
        bmp,
        png
    };
}
