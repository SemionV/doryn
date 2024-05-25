#pragma once

#include "glfwWindow.h"
#include "base/domain/entity.h"
#include "base/domain/events/hub.h"

namespace dory::openGL
{
    template<class TDataContext, typename TWindowRepository>
    class GlfwWindowControllerFactory;

    template<class TDataContext, typename TWindowRepository>
    class GlfwWindowController: public domain::Controller<TDataContext>
    {
    private:
        using WindowRepositoryType = domain::IEntityRepository<TWindowRepository, openGL::GlfwWindow, domain::entity::IdType>;
        WindowRepositoryType& windowRepository;

        using WindowEventHubType = domain::events::window::Dispatcher<TDataContext>;
        WindowEventHubType& windowEventHubDispatcher;

    public:
        using FactoryType = GlfwWindowControllerFactory<TDataContext, TWindowRepository>;

        explicit GlfwWindowController(WindowRepositoryType& windowRepository,
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
                    this->windowEventHubDispatcher.charge(domain::events::window::Close(window.id));
                    glfwSetWindowShouldClose(window.handler, 0);
                }
            });

            windowEventHubDispatcher.fireAll(context);
        }
    };

    template<class TDataContext, typename TWindowRepository>
    class GlfwWindowControllerFactory: public IServiceFactory<GlfwWindowControllerFactory<TDataContext, TWindowRepository>>
    {
    private:
        using ControllerInterfaceType = domain::Controller<TDataContext>;

        using WindowRepositoryType = domain::IEntityRepository<TWindowRepository, openGL::GlfwWindow, domain::entity::IdType>;
        WindowRepositoryType& windowRepository;

        using WindowEventHubType = domain::events::window::Dispatcher<TDataContext>;
        WindowEventHubType& windowEventHubDispatcher;

    public:
        explicit GlfwWindowControllerFactory(WindowRepositoryType& windowRepository,
                                             WindowEventHubType& windowEventHubDispatcher):
                windowRepository(windowRepository),
                windowEventHubDispatcher(windowEventHubDispatcher)
        {}

        std::shared_ptr<ControllerInterfaceType> createInstanceImpl()
        {
            return std::static_pointer_cast<ControllerInterfaceType>(std::make_shared<GlfwWindowController<TDataContext, TWindowRepository>>(windowRepository, windowEventHubDispatcher));
        }
    };
}