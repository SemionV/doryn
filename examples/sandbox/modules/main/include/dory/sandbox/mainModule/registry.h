#pragma once

#include <dory/registry.h>
#include <dory/engine/resources/localization.h>
#include <dory/engineObjects/resources/dataContext.h>

namespace dory::sandbox
{
    class Registry
    {
    public:
        using DataContextType = engineResources::DataContext<ProjectDataContext>;
        using ConfigurationType = engineResources::configuration::Configuration;
        using RepositoryTypes = dory::RepositoryTypeRegistry<ProjectDataContext>;
        using ServiceTypes = dory::ServiceTypeRegistry<RepositoryTypes, ProjectDataContext>;

        dory::EventLayer<dory::EventTypeRegistry<ProjectDataContext>> events;
        dory::DeviceLayer<dory::DeviceTypeRegistry<ProjectDataContext>> devices;
        dory::RepositoryLayer<RepositoryTypes> repositories;
        dory::ServiceLayer<ServiceTypes> services;

        explicit Registry():
                devices { events },
                services { events, repositories }
        {}
    };
}