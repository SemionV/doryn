#include "dependencies.h"
#include "glfwWindowSystem.h"

namespace doryOpenGL
{
    bool GlfwWindowSystem::connect()
    {
        return glfwInit();
    }

    void GlfwWindowSystem::disconnect()
    {
        glfwTerminate();
    }

    void GlfwWindowSystem::update()
    {
        glfwPollEvents();
    }

    std::shared_ptr<dory::Window> GlfwWindowSystem::createWindow(const GlfwWindowParameters& parameters)
    {
        auto window = std::make_shared<GlfwWindow>();

        window->handler = glfwCreateWindow(640, 480, "dory - glfw window", NULL, NULL);

        return window;
    }
}