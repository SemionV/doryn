#pragma once

#include "domain/engine.h"
#include "domain/services/frameService.h"
#include "domain/events/systemConsoleEventHub.h"
#include "domain/entityRepository.h"
#include "engine/engine.h"

namespace dory
{
    template<typename T>
    struct ServicePolicy
    {
        using Type = T;
    };

    template<typename T>
    struct DefaultServicePolicy: public ServicePolicy<T>
    {
        template<typename TServiceLocator>
        static T create(const TServiceLocator& serviceLocator)
        {
            return T{};
        }
    };

    template<class TDataContext, typename TServicePolicies>
    class ServiceLocator
    {
    private:
        std::shared_ptr<domain::services::IFrameService<TDataContext>> frameService;

        std::shared_ptr<domain::IIdFactory<domain::entity::IdType>> pipelineNodeIdFactory;
        std::shared_ptr<domain::EntityRepository<domain::entity::PipelineNode>> pipelineNodeRepository;
        std::shared_ptr<domain::RepositoryReader<domain::entity::PipelineNode>> pipelineNodeRepositoryReader;

        std::shared_ptr<domain::services::PipelineService> pipelineService;

        std::shared_ptr<domain::events::EngineEventHubDispatcher<TDataContext>> engineEventHub;
        std::shared_ptr<domain::events::SystemConsoleEventHubDispatcher<TDataContext>> consoleEventHub;

    public:
        typename TServicePolicies::ConfigurationServicePolicy::Type configuration;

        typename TServicePolicies::CameraRepositoryServicePolicy::Type cameraRepository;
        typename TServicePolicies::IdFactoryServicePolicy::Type cameraIdFactory;

        typename TServicePolicies::IdFactoryServicePolicy::Type viewIdFactory;
        typename TServicePolicies::ViewRepositoryServicePolicy::Type viewRepository;

    protected:
        virtual void configureServices() = 0;

        virtual std::shared_ptr<domain::services::IFrameService<TDataContext>> buildFrameService() = 0;

        virtual std::shared_ptr<domain::IIdFactory<domain::entity::IdType>> buildPipelineNodeIdFactory() = 0;
        virtual std::shared_ptr<domain::EntityRepository<domain::entity::PipelineNode>> buildPipelineNodeRepository() = 0;
        virtual std::shared_ptr<domain::RepositoryReader<domain::entity::PipelineNode>> buildPipelineNodeRepositoryReader() = 0;

        virtual std::shared_ptr<domain::services::PipelineService> buildPipelineService() = 0;

        virtual std::shared_ptr<domain::events::EngineEventHubDispatcher<TDataContext>> buildEngineEventHub() = 0;
        virtual std::shared_ptr<domain::events::SystemConsoleEventHubDispatcher<TDataContext>> buildConsoleEventHub() = 0;

    public:
        ServiceLocator():
                configuration(TServicePolicies::ConfigurationServicePolicy::create(*this)),
                cameraRepository(TServicePolicies::CameraRepositoryServicePolicy::create(*this)),
                cameraIdFactory(TServicePolicies::IdFactoryServicePolicy::create(*this)),
                viewRepository(TServicePolicies::ViewRepositoryServicePolicy::create(*this)),
                viewIdFactory(TServicePolicies::IdFactoryServicePolicy::create(*this))
        {}

        virtual ~ServiceLocator() = default;

        void configure()
        {
            frameService = buildFrameService();

            pipelineNodeIdFactory = buildPipelineNodeIdFactory();
            pipelineNodeRepository = buildPipelineNodeRepository();
            pipelineNodeRepositoryReader = buildPipelineNodeRepositoryReader();

            pipelineService = buildPipelineService();

            engineEventHub = buildEngineEventHub();
            consoleEventHub = buildConsoleEventHub();

            configureServices();
        }

        std::shared_ptr<domain::services::IFrameService<TDataContext>> getFrameService() { return frameService;}

        std::shared_ptr<domain::IIdFactory<domain::entity::IdType>> getPipelineNodeIdFactory() {return pipelineNodeIdFactory;}
        std::shared_ptr<domain::EntityRepository<domain::entity::PipelineNode>> getPipelineNodeRepository() {return pipelineNodeRepository;}
        std::shared_ptr<domain::RepositoryReader<domain::entity::PipelineNode>> getPipelineNodeRepositoryReader() {return pipelineNodeRepositoryReader;}

        std::shared_ptr<domain::services::PipelineService> getPipelineService() { return pipelineService;}

        std::shared_ptr<domain::events::EngineEventHubDispatcher<TDataContext>> getEngineEventHub() { return engineEventHub;}
        std::shared_ptr<domain::events::SystemConsoleEventHubDispatcher<TDataContext>> getConsoleEventHub() {return consoleEventHub;}
    };
}