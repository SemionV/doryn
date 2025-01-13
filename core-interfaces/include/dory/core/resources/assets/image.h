#pragma once
#include <vector>
#include <string>

namespace dory::core::resources::assets
{
    struct Image
    {
        std::string name;
        std::vector<unsigned char> data {};
        unsigned int width {};
        unsigned int height {};
        unsigned char components {};
    };
}
