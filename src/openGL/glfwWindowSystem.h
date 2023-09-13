#pragma once

#include "vendor/glfw/include/GLFW/glfw3.h"
#include "glfwWindowParameters.h"
#include "glfwWindow.h"
#include "events/glfwWindowEventHub.h"

namespace dory::openGL
{
    class DORY_API GlfwWindowSystem: public dory::WindowSystem<GlfwWindowParameters, GlfwWindow>
    {
        private:
            std::shared_ptr<GlfwWindowEventHubDispatcher> eventHub;

        public:
            GlfwWindowSystem(std::shared_ptr<GlfwWindowEventHubDispatcher> eventHub):
                eventHub(eventHub)
            {
            }

            virtual bool connect() override;
            virtual void disconnect() override;
            virtual void update() override;
            void submitEvents(dory::DataContext& context) override;
            
            virtual std::shared_ptr<GlfwWindow> createWindow(const GlfwWindowParameters& parameters) override;
            virtual void closeWindow(std::shared_ptr<GlfwWindow> window) override;
    };
}