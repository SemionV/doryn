#pragma once

#include "glfwWindow.h"
#include "events/glfwWindowEventHub.h"
#include "events/closeWindowEventData.h"

namespace dory::openGL
{
    class GlfwWindowController: public Controller
    {
        private:
            std::shared_ptr<EntityRepository<GlfwWindow>> windowRepository;
            std::shared_ptr<GlfwWindowEventHubDispatcher> eventHub;

        public:
            GlfwWindowController(std::shared_ptr<EntityRepository<GlfwWindow>> windowRepository,
                std::shared_ptr<GlfwWindowEventHubDispatcher> eventHub);

            bool initialize(DataContext& context) override;
            void stop(DataContext& context) override;
            void update(const TimeSpan& timeStep, DataContext& context) override;
    };
}