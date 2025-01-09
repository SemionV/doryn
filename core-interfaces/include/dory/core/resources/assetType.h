#pragma once

#include <string>

namespace dory::core::resources
{
    struct AssetTypeName
    {
        const constexpr static std::string_view extension = "extension";
        const constexpr static std::string_view mesh = "mesh";
        const constexpr static std::string_view material = "material";
        const constexpr static std::string_view shader = "shader";
    };

    enum class AssetFileFormat
    {
        bmp,
        png
    };
}
