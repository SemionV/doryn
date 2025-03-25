#pragma once

#include <dory/core/devices/iWindowSystemDevice.h>
#include <GLFW/glfw3.h>
#include <dory/core/resources/glfwWindow.h>

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

        static resources::entities::GlfwWindow* getWindow(Registry& registry, GLFWwindow* windowHandler);
        static events::KeyCode getKeyCode(int glfwKey);
        static events::KeyAction getKeyAction(int action);
        static events::ModificationKeysState getModKeysState(int mods);

        static void framebufferSizeCallback(GLFWwindow* windowHandler, int width, int height);
        static void keyCallback(GLFWwindow* windowHandler, int key, int scancode, int action, int mods);
        static void cursorPosCallback(GLFWwindow* windowHandler, double x, double y);
        static void mouseButtonCallback(GLFWwindow* windowHandler, int button, int action, int mods);

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
