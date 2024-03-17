#pragma once

#include "base/serviceContainer.h"
#include "base/configuration/fileSystemBasedConfiguration.h"
#include "base/domain/events/engineEventHub.h"
#include "base/domain/entity.h"
#include "base/domain/idFactory.h"
#include "base/domain/entityRepository.h"
#include "base/domain/engine.h"
#include "openGL/glfwWindow.h"
#include "base/domain/services/frameService.h"

namespace dory
{
    template<>
    struct ServiceInstantiator<dory::configuration::FileSystemBasedConfiguration>
    {
        template<typename TServiceContainer>
        static decltype(auto) createInstance(TServiceContainer& services)
        {
            return dory::configuration::FileSystemBasedConfiguration{"configuration"};
        }
    };
}


namespace testApp
{
    namespace entity = dory::domain::entity;
    namespace domain = dory::domain;

    template<typename TDataContext>
    struct ServiceDependencies
    {
        using IdType = entity::IdType;
        using ConfigurationServiceType = dory::configuration::FileSystemBasedConfiguration;
        using PipelineNodeRepositoryType = domain::EntityRepository2<entity::PipelineNode, IdType>;
        using CameraRepositoryType = domain::EntityRepository2<entity::Camera, IdType>;
        using ViewRepositoryType = domain::EntityRepository2<entity::View, IdType>;
        using WindowRepositoryType = domain::EntityRepository2<dory::openGL::GlfwWindow, IdType>;
        using PipelineServiceType = domain::services::PipelineService2<PipelineNodeRepositoryType>;
        using EngineType = domain::Engine2<TDataContext, PipelineServiceType>;
        using FrameServiceType = services::BasicFrameService2<TDataContext, EngineType>;

        using ConfigurationService = dory::Singleton<ConfigurationServiceType>;

        using EngineEventHubDispatcher = dory::Singleton<events::EngineEventHubDispatcher<TDataContext>>;
        using EngineEventHub = dory::Reference<EngineEventHubDispatcher, events::EngineEventHub<TDataContext>>;
        using ConsoleEventHubDispatcher = dory::Singleton<events::SystemConsoleEventHubDispatcher<TDataContext>>;
        using ConsoleEventHub = dory::Reference<ConsoleEventHubDispatcher, events::SystemConsoleEventHub<TDataContext>>;
        using WindowEventHubDispatcher = dory::Singleton<events::WindowEventHubDispatcher<TDataContext>>;
        using WindowEventHub = dory::Reference<WindowEventHubDispatcher, events::WindowEventHub<TDataContext>>;

        using PipelineNodeRepository = dory::Singleton<PipelineNodeRepositoryType, domain::IEntityRepository<PipelineNodeRepositoryType, entity::PipelineNode, IdType>>;
        using CameraRepository = dory::Singleton<CameraRepositoryType, domain::IEntityRepository<CameraRepositoryType, entity::Camera, IdType>>;
        using ViewRepository = dory::Singleton<ViewRepositoryType, domain::IEntityRepository<ViewRepositoryType, entity::View, IdType>>;
        using WindowRepository = dory::Singleton<WindowRepositoryType, domain::IEntityRepository<WindowRepositoryType, dory::openGL::GlfwWindow, IdType>>;

        using PipelineService = dory::Singleton<PipelineServiceType, services::IPipelineService<PipelineServiceType>, DependencyList<PipelineNodeRepository>>;

        using Engine = dory::Singleton<EngineType, domain::IEngine<EngineType, TDataContext>, DependencyList<EngineEventHubDispatcher, PipelineService>>;
        using FrameService = dory::Singleton<FrameServiceType, services::IFrameService<FrameServiceType, TDataContext>, DependencyList<Engine>>;

        using ServiceContainerType = dory::ServiceContainer<
                ConfigurationService,
                EngineEventHubDispatcher,
                EngineEventHub,
                ConsoleEventHubDispatcher,
                ConsoleEventHub,
                WindowEventHubDispatcher,
                WindowEventHub,
                PipelineNodeRepository,
                CameraRepository,
                ViewRepository,
                WindowRepository,
                PipelineService,
                Engine,
                FrameService>;
    };

    class Project2
    {
    private:
        ServiceDependencies<ProjectDataContext>::ServiceContainerType& services;

    public:
        explicit Project2(ServiceDependencies<ProjectDataContext>::ServiceContainerType &services):
            services(services)
        {}
    };
}