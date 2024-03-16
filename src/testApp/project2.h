#pragma once

#include "base/serviceContainer.h"
#include "base/configuration/fileSystemBasedConfiguration.h"
#include "base/domain/events/engineEventHub.h"
#include "base/domain/entity.h"
#include "base/domain/idFactory.h"
#include "base/domain/entityRepository.h"
#include "base/domain/engine.h"

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
        using PipelineNodeRepositoryType = domain::EntityRepository2<entity::PipelineNode, IdType>;
        using CameraRepositoryType = domain::EntityRepository2<entity::Camera, IdType>;
        using PipelineServiceType = domain::services::PipelineService2<PipelineNodeRepositoryType>;
        using EngineType = domain::Engine2<TDataContext, PipelineServiceType>;

        using ConfigurationService = dory::Singleton<dory::configuration::FileSystemBasedConfiguration>;
        using EngineEventHubDispatcher = dory::Singleton<domain::events::EngineEventHubDispatcher<TDataContext>>;
        using EngineEventHub = dory::Reference<EngineEventHubDispatcher, domain::events::EngineEventHub<TDataContext>>;

        using PipelineNodeRepository = dory::Singleton<PipelineNodeRepositoryType, domain::IEntityRepository<PipelineNodeRepositoryType, entity::PipelineNode, IdType>>;
        using CameraRepository = dory::Singleton<CameraRepositoryType, domain::IEntityRepository<CameraRepositoryType, entity::Camera, IdType>>;

        using PipelineService = dory::Singleton<PipelineServiceType, services::IPipelineService<PipelineServiceType>, PipelineNodeRepository>;

        using Engine = dory::Singleton<EngineType, domain::IEngine<EngineType, TDataContext>, EngineEventHubDispatcher, PipelineService>;

        using ServiceContainerType = dory::ServiceContainer<
                ConfigurationService,
                EngineEventHubDispatcher,
                EngineEventHub,
                PipelineNodeRepository,
                CameraRepository,
                PipelineService,
                Engine>;
    };
}