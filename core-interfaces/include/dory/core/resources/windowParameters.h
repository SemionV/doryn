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
        IdType graphicalContextId;
        unsigned short sampling { 0 };
        bool vSync {};
        bool fullScreen {};
    };
}
