#pragma once

#include "vendor/glfw/include/GLFW/glfw3.h"
#include "glfwWindowParameters.h"
#include "glfwWindow.h"

namespace doryOpenGL
{
    class DORY_API GlfwWindowSystem: public dory::WindowSystem<GlfwWindowParameters>
    {
        private:
            std::shared_ptr<dory::SystemWindowEventHubDispatcher> eventHub;

        public:
            GlfwWindowSystem(std::shared_ptr<dory::SystemWindowEventHubDispatcher> eventHub):
                eventHub(eventHub)
            {
            }

            virtual bool connect() override;
            virtual void disconnect() override;
            virtual void update() override;
            virtual std::shared_ptr<dory::Window> createWindow(const GlfwWindowParameters& parameters) override;
    };
}