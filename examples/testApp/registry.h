#pragma once

#include "dependencies.h"
#include "projectData.h"

namespace testApp
{
    class Registry
    {
    public:
        using DataContextType = ProjectDataContext;
        using ConfigurationType = dory::configuration::Configuration;
        using LocalizationType = dory::domain::resources::Localization;
        using RepositoryTypes = dory::RepositoryTypeRegistry<DataContextType>;
        using ServiceTypes = dory::ServiceTypeRegistry<RepositoryTypes, DataContextType>;

        dory::EventLayer<dory::EventTypeRegistry<DataContextType>> events;
        dory::DeviceLayer<dory::DeviceTypeRegistry<DataContextType>> devices;
        dory::RepositoryLayer<RepositoryTypes> repositories;
        dory::ServiceLayer<ServiceTypes> services;
        dory::ManagerLayer<dory::ManagerTypeRegistry<RepositoryTypes, ServiceTypes, DataContextType>> managers;

        explicit Registry(const ConfigurationType& configuration):
                devices { events },
                services { configuration, events, repositories },
                managers { repositories, services }
        {}
    };
}