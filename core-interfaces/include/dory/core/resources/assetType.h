#pragma once

#include <string>

namespace dory::core::resources
{
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
