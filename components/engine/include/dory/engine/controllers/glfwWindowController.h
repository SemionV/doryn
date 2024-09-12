#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "dory/engine/entity.h"
#include "dory/engine/events/hub.h"
#include "dory/engine/controllers/controller.h"
#include "dory/engine/entityRepository.h"
#include "dory/engine/resources/glfwWindow.h"

namespace dory::opengl
{
    template<class TDataContext, typename TWindowRepository>
    class GlfwWindowControllerFactory;

    template<class TDataContext, typename TWindowRepository>
    class GlfwWindowController: public domain::Controller<TDataContext>
    {
    private:
        using WindowRepositoryType = domain::IEntityRepository<TWindowRepository, GlfwWindow, domain::entity::IdType>;
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

        using WindowRepositoryType = domain::IEntityRepository<TWindowRepository, GlfwWindow, domain::entity::IdType>;
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