#pragma once

#include "glfwWindow.h"

namespace dory::openGL
{
    class GlfwWindowController: public Controller
    {
        private:
            std::shared_ptr<EntityAccessor<GlfwWindow>> windowRepository;
            std::shared_ptr<WindowEventHubDispatcher> eventHub;

        public:
            GlfwWindowController(std::shared_ptr<EntityAccessor<GlfwWindow>> windowRepository,
                std::shared_ptr<WindowEventHubDispatcher> eventHub);

            bool initialize(DataContext& context) override;
            void stop(DataContext& context) override;
            void update(const int referenceId, const TimeSpan& timeStep, DataContext& context) override;
    };
}