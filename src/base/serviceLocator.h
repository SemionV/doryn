#pragma once

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

    template<typename T>
    struct ConstructServicePolicy: ServicePolicy<T>
    {
        template<typename TServiceLocator>
        static decltype(auto) create(const TServiceLocator& serviceLocator)
        {
            return T{serviceLocator};
        }
    };

    template<class TDataContext, typename TServicePolicies>
    class ServiceLocator
    {
    public:
        typename TServicePolicies::ConfigurationServicePolicy::Type configuration;
        typename TServicePolicies::EngineServicePolicy::Type engine;
        typename TServicePolicies::FrameServiceServicePolicy::Type frameService;
        typename TServicePolicies::PipelineServiceServicePolicy::Type pipelineService;

        typename TServicePolicies::EngineEventHubServicePolicy::Type engineEventHub;
        typename TServicePolicies::ConsoleEventHubServicePolicy::Type consoleEventHub;

        typename TServicePolicies::CameraRepositoryServicePolicy::Type cameraRepository;
        typename TServicePolicies::IdFactoryServicePolicy::Type cameraIdFactory;

        typename TServicePolicies::IdFactoryServicePolicy::Type viewIdFactory;
        typename TServicePolicies::ViewRepositoryServicePolicy::Type viewRepository;

        typename TServicePolicies::IdFactoryServicePolicy::Type pipelineNodeIdFactory;
        typename TServicePolicies::PipelineNodeRepositoryServicePolicy::Type pipelineNodeRepository;

    public:
        ServiceLocator():
                configuration(TServicePolicies::ConfigurationServicePolicy::create(*this)),
                engine(TServicePolicies::EngineServicePolicy::create(*this)),
                frameService(TServicePolicies::FrameServiceServicePolicy::create(*this)),
                pipelineService(TServicePolicies::PipelineServiceServicePolicy::create(*this)),
                cameraRepository(TServicePolicies::CameraRepositoryServicePolicy::create(*this)),
                cameraIdFactory(TServicePolicies::IdFactoryServicePolicy::create(*this)),
                viewRepository(TServicePolicies::ViewRepositoryServicePolicy::create(*this)),
                viewIdFactory(TServicePolicies::IdFactoryServicePolicy::create(*this)),
                pipelineNodeRepository(TServicePolicies::PipelineNodeRepositoryServicePolicy::create(*this)),
                pipelineNodeIdFactory(TServicePolicies::IdFactoryServicePolicy::create(*this)),
                engineEventHub(TServicePolicies::EngineEventHubServicePolicy::create(*this)),
                consoleEventHub(TServicePolicies::ConsoleEventHubServicePolicy::create(*this))
        {}
    };
}