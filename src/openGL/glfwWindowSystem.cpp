#include "dependencies.h"
#include "glfwWindowSystem.h"

namespace dory::openGL
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

        std::size_t size = windows.size();
        for(std::size_t i = 0; i < size; ++i)
        {
            auto window = windows[i];
            if(glfwWindowShouldClose(window->handler))
            {
                eventHub->addCase(CloseWindowEventData(window));
                glfwSetWindowShouldClose(window->handler, 0);
            }
        }
    }

    void GlfwWindowSystem::submitEvents(dory::DataContext& context)
    {
        eventHub->submit(context);
    }

    std::shared_ptr<GlfwWindow> GlfwWindowSystem::createWindow(const GlfwWindowParameters& parameters)
    {
        auto handler = glfwCreateWindow(800, 600, "dory - glfw window", NULL, NULL);
        auto window = std::make_shared<GlfwWindow>(getNewWindowId(), handler);
        registerWindow(window);

        return window;
    }

    void GlfwWindowSystem::closeWindow(std::shared_ptr<GlfwWindow> window)
    {
        glfwDestroyWindow(window->handler);
        unregisterWindow(window);
    }
}