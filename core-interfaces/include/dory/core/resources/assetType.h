#pragma once

#include <string>

namespace dory::core::resources
{
    struct AssetTypeName
    {
        const constexpr static std::string_view extension = "extension";
        const constexpr static std::string_view mesh = "mesh";
    };
}
