#pragma once

#include <dory/registry.h>
#include <dory/engine/domain/resources/localization.h>

namespace dory::sandbox
{
    struct ProjectDataContext
    {
    };

    class Registry
    {
    public:
        using DataContextType = dory::domain::DataContext<ProjectDataContext>;
        using ConfigurationType = dory::configuration::Configuration;
        using RepositoryTypes = dory::RepositoryTypeRegistry<ProjectDataContext>;
        using ServiceTypes = dory::ServiceTypeRegistry<RepositoryTypes, ProjectDataContext>;

        dory::EventLayer<dory::EventTypeRegistry<ProjectDataContext>> events;
        dory::DeviceLayer<dory::DeviceTypeRegistry<ProjectDataContext>> devices;
        dory::RepositoryLayer<RepositoryTypes> repositories;
        dory::ServiceLayer<ServiceTypes> services;
        dory::ManagerLayer<dory::ManagerTypeRegistry<RepositoryTypes, ServiceTypes, ProjectDataContext>> managers;

        explicit Registry():
                devices { events },
                services { events, repositories },
                managers { repositories, services }
        {}
    };
}