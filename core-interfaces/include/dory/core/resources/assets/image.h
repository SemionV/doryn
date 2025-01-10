#pragma once

namespace dory::core::resources::assets
{
    struct Image
    {
        std::vector<unsigned char> data {};
        unsigned int width {};
        unsigned int height {};
        unsigned char components {};
    };
}