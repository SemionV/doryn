#pragma once

#include "openGL/dependencies.h"
#include "openGL/glfwWindow.h"

namespace dory::openGL
{
    struct CloseWindowEventData
    {
        public:
            const std::shared_ptr<GlfwWindow> window;

        public:
            CloseWindowEventData(std::shared_ptr<GlfwWindow> window):
                window(window)
            {                
            }
    };
}