#pragma once

#include "dependencies.h"
#include "glfwWindowParameters.h"

namespace dory::openGL
{
    class GlfwWindowFactory
    {
        public:
            static GLFWwindow* createWindow(GlfwWindowParameters windowParameters)
            {
                return glfwCreateWindow(800, 600, "dory - glfw window", NULL, NULL);
            }
    };
}