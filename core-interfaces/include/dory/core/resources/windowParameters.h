#pragma once

#include <string>

namespace dory::core::resources
{
    struct WindowParameters
    {
        unsigned int width;
        unsigned int height;
        std::string title;
    };
}
