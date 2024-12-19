#pragma once

#include <dory/core/devices/iWindowSystemDevice.h>
#include <GLFW/glfw3.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::devices
{
    class GlfwWindowSystemDevice: public IWindowSystemDevice
    {
    private:
        Registry& _registry;

        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
        void onWindowResize(GLFWwindow* window, int width, int height);

    public:
        explicit GlfwWindowSystemDevice(Registry& registry);

        void connect(resources::DataContext& context) override;
        void disconnect(resources::DataContext& context) override;
        void pollEvents(resources::DataContext& context) override;

        void setupWindow(resources::entities::Window& window, const resources::WindowParameters& parameters) override;
        void closeWindow(const resources::entities::Window& window) override;
        void setCurrentWindow(const resources::entities::Window& window) override;
        void swapWindowBuffers(const resources::entities::Window& window) override;
    };
}
