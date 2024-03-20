#pragma once

#include "glfwWindow.h"
#include "base/domain/entity.h"

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