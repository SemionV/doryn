#pragma once

#include "dependencies.h"
#include "configuration/configuration.h"
#include "domain/engine.h"
#include "domain/services/frameService.h"
#include "domain/dataContext.h"
#include "domain/events/systemConsoleEventHub.h"
#include "domain/entityRepository.h"

namespace dory
{
    template<typename T>
    struct ServicePolicy
    {
        using Type = T;
    };

    struct DefaultServicePolicies
    {
        using ConfigurationServicePolicy = ServicePolicy<void>;
    };

    template<class TDataContext, typename TServicePolicies = DefaultServicePolicies>
    class ServiceLocator
    {
    private:
        std::shared_ptr<domain::services::IFrameService<TDataContext>> frameService;

        std::shared_ptr<domain::IIdFactory<domain::entity::IdType>> cameraIdFactory;
        std::shared_ptr<domain::EntityRepository<domain::entity::Camera>> cameraRepository;
        std::shared_ptr<domain::RepositoryReader<domain::entity::Camera>> cameraRepositoryReader;

        std::shared_ptr<domain::IIdFactory<domain::entity::IdType>> viewIdFactory;
        std::shared_ptr<domain::EntityRepository<domain::entity::View>> viewRepository;
        std::shared_ptr<domain::RepositoryReader<domain::entity::View>> viewRepositoryReader;

        std::shared_ptr<domain::IIdFactory<domain::entity::IdType>> pipelineNodeIdFactory;
        std::shared_ptr<domain::EntityRepository<domain::entity::PipelineNode>> pipelineNodeRepository;
        std::shared_ptr<domain::RepositoryReader<domain::entity::PipelineNode>> pipelineNodeRepositoryReader;

        std::shared_ptr<domain::services::PipelineService> pipelineService;

        std::shared_ptr<domain::events::EngineEventHubDispatcher<TDataContext>> engineEventHub;
        std::shared_ptr<domain::events::SystemConsoleEventHubDispatcher<TDataContext>> consoleEventHub;

    public:
        TServicePolicies::ConfigurationServicePolicy::Type configuration;

    protected:
        virtual void configureServices() = 0;

        virtual std::shared_ptr<domain::services::IFrameService<TDataContext>> buildFrameService() = 0;

        virtual std::shared_ptr<domain::IIdFactory<domain::entity::IdType>> buildCameraIdFactory() = 0;
        virtual std::shared_ptr<domain::EntityRepository<domain::entity::Camera>> buildCameraRepository() = 0;
        virtual std::shared_ptr<domain::RepositoryReader<domain::entity::Camera>> buildCameraRepositoryReader() = 0;

        virtual std::shared_ptr<domain::IIdFactory<domain::entity::IdType>> buildViewIdFactory() = 0;
        virtual std::shared_ptr<domain::EntityRepository<domain::entity::View>> buildViewRepository() = 0;
        virtual std::shared_ptr<domain::RepositoryReader<domain::entity::View>> buildViewRepositoryReader() = 0;

        virtual std::shared_ptr<domain::IIdFactory<domain::entity::IdType>> buildPipelineNodeIdFactory() = 0;
        virtual std::shared_ptr<domain::EntityRepository<domain::entity::PipelineNode>> buildPipelineNodeRepository() = 0;
        virtual std::shared_ptr<domain::RepositoryReader<domain::entity::PipelineNode>> buildPipelineNodeRepositoryReader() = 0;

        virtual std::shared_ptr<domain::services::PipelineService> buildPipelineService() = 0;

        virtual std::shared_ptr<domain::events::EngineEventHubDispatcher<TDataContext>> buildEngineEventHub() = 0;
        virtual std::shared_ptr<domain::events::SystemConsoleEventHubDispatcher<TDataContext>> buildConsoleEventHub() = 0;

    public:
        ServiceLocator():
                configuration(TServicePolicies::ConfigurationServicePolicy::create(*this))
        {}

        virtual ~ServiceLocator() = default;

        void configure()
        {
            frameService = buildFrameService();

            cameraIdFactory = buildCameraIdFactory();
            cameraRepository = buildCameraRepository();
            cameraRepositoryReader = buildCameraRepositoryReader();

            viewIdFactory = buildViewIdFactory();
            viewRepository = buildViewRepository();
            viewRepositoryReader = buildViewRepositoryReader();

            pipelineNodeIdFactory = buildPipelineNodeIdFactory();
            pipelineNodeRepository = buildPipelineNodeRepository();
            pipelineNodeRepositoryReader = buildPipelineNodeRepositoryReader();

            pipelineService = buildPipelineService();

            engineEventHub = buildEngineEventHub();
            consoleEventHub = buildConsoleEventHub();

            configureServices();
        }

        std::shared_ptr<domain::services::IFrameService<TDataContext>> getFrameService() { return frameService;}

        std::shared_ptr<domain::IIdFactory<domain::entity::IdType>> getCameraIdFactory() {return cameraIdFactory;}
        std::shared_ptr<domain::EntityRepository<domain::entity::Camera>> getCameraRepository() {return cameraRepository;}
        std::shared_ptr<domain::RepositoryReader<domain::entity::Camera>> getCameraRepositoryReader() {return cameraRepositoryReader;}

        std::shared_ptr<domain::IIdFactory<domain::entity::IdType>> getViewIdFactory() {return viewIdFactory;};
        std::shared_ptr<domain::EntityRepository<domain::entity::View>> getViewRepository() {return viewRepository;}
        std::shared_ptr<domain::RepositoryReader<domain::entity::View>> getViewRepositoryReader() {return viewRepositoryReader;}

        std::shared_ptr<domain::IIdFactory<domain::entity::IdType>> getPipelineNodeIdFactory() {return pipelineNodeIdFactory;}
        std::shared_ptr<domain::EntityRepository<domain::entity::PipelineNode>> getPipelineNodeRepository() {return pipelineNodeRepository;}
        std::shared_ptr<domain::RepositoryReader<domain::entity::PipelineNode>> getPipelineNodeRepositoryReader() {return pipelineNodeRepositoryReader;}

        std::shared_ptr<domain::services::PipelineService> getPipelineService() { return pipelineService;}

        std::shared_ptr<domain::events::EngineEventHubDispatcher<TDataContext>> getEngineEventHub() { return engineEventHub;}
        std::shared_ptr<domain::events::SystemConsoleEventHubDispatcher<TDataContext>> getConsoleEventHub() {return consoleEventHub;}
    };
}