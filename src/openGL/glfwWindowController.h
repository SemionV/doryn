#pragma once

#include "glfwWindow.h"

namespace dory::openGL
{
    class GlfwWindowController: public domain::Controller
    {
        private:
            std::shared_ptr<domain::RepositoryReader<GlfwWindow>> windowRepository;
            std::shared_ptr<domain::events::WindowEventHubDispatcher> eventHub;

        public:
            GlfwWindowController(std::shared_ptr<domain::RepositoryReader<GlfwWindow>> windowRepository,
                std::shared_ptr<domain::events::WindowEventHubDispatcher> eventHub);

            bool initialize(domain::entity::IdType referenceId, domain::DataContext& context) override;
            void stop(domain::entity::IdType referenceId, domain::DataContext& context) override;
            void update(domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, domain::DataContext& context) override;
    };
}