#pragma once

#include "base/serviceContainer.h"
#include "base/configuration/fileSystemBasedConfiguration.h"
#include "base/domain/events/engineEventHub.h"
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
    template<typename TDataContext>
    struct ServiceDependencies
    {
        using ConfigurationService = dory::Singleton<dory::configuration::FileSystemBasedConfiguration>;
        using EngineEventHubDispatcher = dory::Singleton<dory::domain::events::EngineEventHubDispatcher<TDataContext>>;
        using Engine = dory::Singleton<dory::domain::Engine2<TDataContext>, dory::domain::Engine2<TDataContext>, EngineEventHubDispatcher>;

        using ServiceContainerType = dory::ServiceContainer<
                ConfigurationService,
                EngineEventHubDispatcher,
                Engine>;
    };
}