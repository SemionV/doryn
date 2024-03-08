#pragma once

#include "glfwWindow.h"

namespace dory::openGL
{
    template<class TDataContext, typename TServiceLocator>
    class GlfwWindowController: public domain::Controller<TDataContext, TServiceLocator>
    {
        public:
             explicit GlfwWindowController(TServiceLocator& serviceLocator):
                     domain::Controller<TDataContext, TServiceLocator>(serviceLocator)
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

                this->services.windowRepository.forEach([this](auto& window)
                {
                    if(glfwWindowShouldClose(window.handler))
                    {
                        this->services.windowEventHub.addCase(domain::events::CloseWindowEventData(window.id));
                        glfwSetWindowShouldClose(window.handler, 0);
                    }
                });

                this->services.windowEventHub.submit(context);
            }
    };
}