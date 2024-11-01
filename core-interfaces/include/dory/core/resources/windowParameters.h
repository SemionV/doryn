#pragma once

#include <string>
#include "graphicalSystem.h"

namespace dory::core::resources
{
    struct WindowParameters
    {
        unsigned int width;
        unsigned int height;
        std::string title;
        GraphicalSystem graphicalSystem;
    };
}
