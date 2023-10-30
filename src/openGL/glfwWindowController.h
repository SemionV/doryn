#pragma once

#include "glfwWindow.h"

namespace dory::openGL
{
    template<class TDataContext>
    class GlfwWindowController: public domain::Controller<TDataContext>
    {
        private:
            std::shared_ptr<domain::RepositoryReader<GlfwWindow>> windowRepository;
            std::shared_ptr<domain::events::WindowEventHubDispatcher<TDataContext>> eventHub;

        public:
             GlfwWindowController(std::shared_ptr<domain::RepositoryReader<GlfwWindow>> windowRepository,
                std::shared_ptr<domain::events::WindowEventHubDispatcher<TDataContext>> eventHub):
                    windowRepository(windowRepository),
                    eventHub(eventHub)
            {}

            bool initialize(domain::entity::IdType referenceId, TDataContext& context) override
            {
                return glfwInit();
            };

            void stop(domain::entity::IdType referenceId, TDataContext& context) override
            {
                glfwTerminate();
            }

            void update(domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, TDataContext& context) override
            {
                glfwPollEvents();

                windowRepository->forEach([this](auto& window)
                {
                    if(glfwWindowShouldClose(window.handler))
                    {
                        eventHub->addCase(domain::events::CloseWindowEventData(window.id));
                        glfwSetWindowShouldClose(window.handler, 0);
                    }
                });

                eventHub->submit(context);
            }
    };
}