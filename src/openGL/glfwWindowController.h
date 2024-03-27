#pragma once

#include "glfwWindow.h"
#include "base/domain/entity.h"
#include "base/domain/events/windowEventHub.h"

namespace dory::openGL
{
    template<class TDataContext, typename TWindowRepository>
    class GlfwWindowController2: public domain::Controller2<TDataContext>
    {
    private:
        using WindowRepositoryType = domain::IEntityRepository<TWindowRepository, openGL::GlfwWindow, domain::entity::IdType>;
        WindowRepositoryType& windowRepository;

        using WindowEventHubType = domain::events::WindowEventHubDispatcher<TDataContext>;
        WindowEventHubType& windowEventHubDispatcher;

    public:
        explicit GlfwWindowController2(WindowRepositoryType& windowRepository,
                                       WindowEventHubType& windowEventHubDispatcher):
                windowRepository(windowRepository),
                windowEventHubDispatcher(windowEventHubDispatcher)
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

            windowRepository.forEach([this](auto& window)
            {
                if(glfwWindowShouldClose(window.handler))
                {
                    this->windowEventHubDispatcher.addCase(domain::events::CloseWindowEventData(window.id));
                    glfwSetWindowShouldClose(window.handler, 0);
                }
            });

            windowEventHubDispatcher.submit(context);
        }
    };
}