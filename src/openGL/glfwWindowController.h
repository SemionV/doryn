#pragma once

#include "glfwWindow.h"

namespace dory::openGL
{
    class GlfwWindowController: public Controller
    {
        private:
            std::shared_ptr<RepositoryReader<GlfwWindow>> windowRepository;
            std::shared_ptr<events::WindowEventHubDispatcher> eventHub;

        public:
            GlfwWindowController(std::shared_ptr<RepositoryReader<GlfwWindow>> windowRepository,
                std::shared_ptr<events::WindowEventHubDispatcher> eventHub);

            bool initialize(domain::entity::IdType referenceId, DataContext& context) override;
            void stop(domain::entity::IdType referenceId, DataContext& context) override;
            void update(domain::entity::IdType referenceId, const TimeSpan& timeStep, DataContext& context) override;
    };
}