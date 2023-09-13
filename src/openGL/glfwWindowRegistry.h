#pragma once

#include "vendor/glfw/include/GLFW/glfw3.h"
#include "glfwWindowParameters.h"
#include "glfwWindow.h"

namespace dory::openGL
{
    class GlfwWindowRegistry: public WindowRegistry<GlfwWindow, GlfwWindowParameters>
    {
        protected:
            void initializeWindow(GlfwWindow& window, const GlfwWindowParameters& windowParameters) override;
    };
}