#pragma once

#include <dory/core/resources/entities/display.h>

namespace dory::core::resources::entities
{
    struct GlfwDisplay: public Display
    {
        GLFWmonitor* glfwMonitor {};
    };
}
