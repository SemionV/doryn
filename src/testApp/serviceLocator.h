#pragma once

#include "base/serviceLocator.h"
#include "projectDataContext.h"
#include "base/domain/engine.h"

namespace testApp
{
    using EntityId = dory::domain::entity::IdType;
    using namespace dory;
    using namespace domain;

    struct ServicePolicies;
    using ServiceLocator = dory::ServiceLocator<ServicePolicies>;

    struct ConfigurationServicePolicy: ServicePolicy<configuration::FileSystemBasedConfiguration>
    {
        template<typename TServiceLocator>
        static Type create(TServiceLocator& serviceLocator)
        {
            return configuration::FileSystemBasedConfiguration{"configuration"};
        }
    };

    struct ServicePolicies
    {
        using ConfigurationServicePolicy = ConfigurationServicePolicy;
        using EngineServicePolicy = ConstructServicePolicy<dory::domain::Engine<ProjectDataContext, ServiceLocator>>;

        using FrameServiceServicePolicy = ConstructServicePolicy<dory::domain::services::BasicFrameService<ProjectDataContext, ServiceLocator>>;
        using PipelineServiceServicePolicy = ConstructServicePolicy<dory::domain::services::PipelineService<ServiceLocator>>;

        using EngineEventHubServicePolicy = DefaultServicePolicy<dory::domain::events::EngineEventHubDispatcher<ProjectDataContext>>;
        using ConsoleEventHubServicePolicy = DefaultServicePolicy<dory::domain::events::SystemConsoleEventHubDispatcher<ProjectDataContext>>;
        using WindowEventHubServicePolicy = DefaultServicePolicy<dory::domain::events::WindowEventHubDispatcher<ProjectDataContext>>;

        using IdFactoryServicePolicy = DefaultServicePolicy<IdFactory<EntityId>>;

        using CameraRepositoryServicePolicy = DefaultServicePolicy<NewEntityRepository<dory::domain::entity::Camera>>;
        using ViewRepositoryServicePolicy = DefaultServicePolicy<NewEntityRepository<dory::domain::entity::View>>;
        using PipelineNodeRepositoryServicePolicy = DefaultServicePolicy<NewEntityRepository<dory::domain::entity::PipelineNode>>;
        using WindowRepositoryServicePolicy = DefaultServicePolicy<NewEntityRepository<dory::openGL::GlfwWindow>>;
    };
}