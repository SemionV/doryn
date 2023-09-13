#include "dependencies.h"
#include "glfwWindowRegistry.h"

namespace dory::openGL
{
    void GlfwWindowRegistry::initializeWindow(GlfwWindow& window, const GlfwWindowParameters& windowParameters)
    {
        auto handler = glfwCreateWindow(800, 600, "dory - glfw window", NULL, NULL);
        window.handler = handler;
    }
}