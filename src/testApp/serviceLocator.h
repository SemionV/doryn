#pragma once

namespace testApp
{
    using EntityId = dory::domain::entity::IdType;
    using namespace dory;
    using namespace domain;

    template<class TDataContext>
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
    };

    template<class TDataContext>
    class ServiceLocator: public dory::ServiceLocator<TDataContext, ServicePolicies>
    {
    private:
        std::shared_ptr<IIdFactory<EntityId>> windowIdFactory;
        std::shared_ptr<EntityRepository<openGL::GlfwWindow>> windowRespository;
        std::shared_ptr<RepositoryReader<openGL::GlfwWindow>> windowRepositoryReader;
        std::shared_ptr<events::WindowEventHubDispatcher<TDataContext>> glfwWindowEventHub;

    public:
        std::shared_ptr<IIdFactory<EntityId>> getWindowIdFactory() {return windowIdFactory;}
        std::shared_ptr<EntityRepository<openGL::GlfwWindow>> getWindowRepository() {return windowRespository;}
        std::shared_ptr<RepositoryReader<openGL::GlfwWindow>> getWindowRepositoryReader() {return windowRepositoryReader;}
        std::shared_ptr<events::WindowEventHubDispatcher<TDataContext>> getGlfwWindowEventHub() {return glfwWindowEventHub;}

    protected:

        void configureServices() override
        {
            windowIdFactory = std::make_shared<NumberIdFactory<EntityId>>();
            windowRespository = std::make_shared<EntityRepository<openGL::GlfwWindow>>();
            windowRepositoryReader = std::make_shared<RepositoryReader<dory::openGL::GlfwWindow>>(windowRespository);
            glfwWindowEventHub = std::make_shared<events::WindowEventHubDispatcher<TDataContext>>();
        }

        std::shared_ptr<domain::services::IFrameService<TDataContext>> buildFrameService() override
        {
            return std::make_shared<services::BasicFrameService<TDataContext>>();
        }

        std::shared_ptr<IIdFactory<dory::entity::IdType>> buildCameraIdFactory() override
        {
            return std::make_shared<NumberIdFactory<dory::entity::IdType>>();
        }

        std::shared_ptr<EntityRepository<dory::domain::entity::Camera>> buildCameraRepository() override
        {
            return std::make_shared<EntityRepository<dory::domain::entity::Camera>>();
        }

        std::shared_ptr<RepositoryReader<dory::domain::entity::Camera>> buildCameraRepositoryReader() override
        {
            auto repository = this->getCameraRepository();
            return std::make_shared<RepositoryReader<dory::domain::entity::Camera>>(repository);
        }

        std::shared_ptr<IIdFactory<dory::domain::entity::IdType>> buildViewIdFactory() override
        {
            return std::make_shared<NumberIdFactory<dory::domain::entity::IdType>>();
        }

        std::shared_ptr<EntityRepository<dory::domain::entity::View>> buildViewRepository() override
        {
            return std::make_shared<EntityRepository<dory::domain::entity::View>>();
        }

        std::shared_ptr<RepositoryReader<dory::domain::entity::View>> buildViewRepositoryReader() override
        {
            auto repository = this->getViewRepository();
            return std::make_shared<RepositoryReader<dory::domain::entity::View>>(repository);
        }

        std::shared_ptr<IIdFactory<dory::domain::entity::IdType>> buildPipelineNodeIdFactory() override
        {
            return std::make_shared<NumberIdFactory<dory::domain::entity::IdType>>();
        }

        std::shared_ptr<EntityRepository<dory::domain::entity::PipelineNode>> buildPipelineNodeRepository() override
        {
            return std::make_shared<EntityRepository<dory::domain::entity::PipelineNode>>();
        }

        std::shared_ptr<RepositoryReader<dory::domain::entity::PipelineNode>> buildPipelineNodeRepositoryReader() override
        {
            auto repository = this->getPipelineNodeRepository();
            return std::make_shared<RepositoryReader<dory::domain::entity::PipelineNode>>(repository);
        }

        std::shared_ptr<services::PipelineService> buildPipelineService() override
        {
            auto pipelineNodeRepositoryReader = this->getPipelineNodeRepositoryReader();
            return std::make_shared<services::PipelineService>(pipelineNodeRepositoryReader);
        }

        std::shared_ptr<events::EngineEventHubDispatcher<TDataContext>> buildEngineEventHub() override
        {
            return std::make_shared<events::EngineEventHubDispatcher<TDataContext>>();
        }

        std::shared_ptr<events::SystemConsoleEventHubDispatcher<TDataContext>> buildConsoleEventHub() override
        {
            return std::make_shared<events::SystemConsoleEventHubDispatcher<TDataContext>>();
        }
    };
}