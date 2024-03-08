#pragma once

#include "base/serviceLocator.h"
#include "projectDataContext.h"
#include "base/domain/engine.h"

namespace testApp
{
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
        using EngineServicePolicy = ConstructServicePolicy<Engine<ProjectDataContext, ServiceLocator>>;

        using FrameServiceServicePolicy = ConstructServicePolicy<services::BasicFrameService<ProjectDataContext, ServiceLocator>>;
        using PipelineServiceServicePolicy = ConstructServicePolicy<services::PipelineService<ServiceLocator>>;

        using EngineEventHubServicePolicy = DefaultServicePolicy<events::EngineEventHubDispatcher<ProjectDataContext>>;
        using ConsoleEventHubServicePolicy = DefaultServicePolicy<events::SystemConsoleEventHubDispatcher<ProjectDataContext>>;
        using WindowEventHubServicePolicy = DefaultServicePolicy<events::WindowEventHubDispatcher<ProjectDataContext>>;

        using IdFactoryServicePolicy = DefaultServicePolicy<IdFactory<domain::entity::IdType>>;

        using CameraRepositoryServicePolicy = DefaultServicePolicy<EntityRepository<domain::entity::Camera>>;
        using ViewRepositoryServicePolicy = DefaultServicePolicy<EntityRepository<domain::entity::View>>;
        using PipelineNodeRepositoryServicePolicy = DefaultServicePolicy<EntityRepository<domain::entity::PipelineNode>>;
        using WindowRepositoryServicePolicy = DefaultServicePolicy<EntityRepository<openGL::GlfwWindow>>;
    };
}