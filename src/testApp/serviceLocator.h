#pragma once

#include "base/serviceLocator.h"
#include "projectDataContext.h"
#include "base/domain/engine.h"

namespace testApp
{
    using EntityId = dory::domain::entity::IdType;
    using namespace dory;
    using namespace domain;

    class ServiceLocator;

    struct ConfigurationServicePolicy: ServicePolicy<configuration::FileSystemBasedConfiguration>
    {
        template<typename TServiceLocator>
        static Type create(const TServiceLocator& serviceLocator)
        {
            return configuration::FileSystemBasedConfiguration{"configuration"};
        }
    };

    struct ServicePolicies
    {
        using ConfigurationServicePolicy = ConfigurationServicePolicy;
        using EngineServicePolicy = ConstructServicePolicy<domain::Engine<ProjectDataContext, ServiceLocator>>;
        using FrameServiceServicePolicy = ConstructServicePolicy<services::BasicFrameService<ProjectDataContext, ServiceLocator>>;
        using EngineEventHubServicePolicy = DefaultServicePolicy<domain::events::EngineEventHubDispatcher<ProjectDataContext>>;
        using ConsoleEventHubServicePolicy = DefaultServicePolicy<domain::events::SystemConsoleEventHubDispatcher<ProjectDataContext>>;
        using IdFactoryServicePolicy = DefaultServicePolicy<NumberIdFactory<EntityId>>;
        using CameraRepositoryServicePolicy = DefaultServicePolicy<NewEntityRepository<dory::domain::entity::Camera>>;
        using ViewRepositoryServicePolicy = DefaultServicePolicy<NewEntityRepository<dory::domain::entity::View>>;
        using PipelineNodeRepositoryServicePolicy = DefaultServicePolicy<NewEntityRepository<dory::domain::entity::PipelineNode>>;
        using PipelineServiceServicePolicy = ConstructServicePolicy<services::PipelineService<ServiceLocator>>;
    };

    class ServiceLocator: public dory::ServiceLocator<ProjectDataContext, ServicePolicies>
    {
    private:
        std::shared_ptr<IIdFactory<EntityId>> windowIdFactory;
        std::shared_ptr<EntityRepository<openGL::GlfwWindow>> windowRespository;
        std::shared_ptr<RepositoryReader<openGL::GlfwWindow>> windowRepositoryReader;
        std::shared_ptr<events::WindowEventHubDispatcher<ProjectDataContext>> glfwWindowEventHub;

    public:
        std::shared_ptr<IIdFactory<EntityId>> getWindowIdFactory() {return windowIdFactory;}
        std::shared_ptr<EntityRepository<openGL::GlfwWindow>> getWindowRepository() {return windowRespository;}
        std::shared_ptr<RepositoryReader<openGL::GlfwWindow>> getWindowRepositoryReader() {return windowRepositoryReader;}
        std::shared_ptr<events::WindowEventHubDispatcher<ProjectDataContext>> getGlfwWindowEventHub() {return glfwWindowEventHub;}

    protected:

        void configureServices()
        {
            windowIdFactory = std::make_shared<NumberIdFactory<EntityId>>();
            windowRespository = std::make_shared<EntityRepository<openGL::GlfwWindow>>();
            windowRepositoryReader = std::make_shared<RepositoryReader<dory::openGL::GlfwWindow>>(windowRespository);
            glfwWindowEventHub = std::make_shared<events::WindowEventHubDispatcher<ProjectDataContext>>();
        }
    };
}