#include "dependencies.h"

namespace dory
{
    template<typename TService, typename... TDependencies>
    struct ServiceInstantiator
    {
        template<typename TServiceContainer>
        static TService getInstance(TServiceContainer& services)
        {
            return TService(services.template get<TDependencies>()...);
        }
    };

    template<typename TService>
    struct ServiceInstantiator<TService>
    {
        template<typename TServiceContainer>
        static TService getInstance(TServiceContainer& services)
        {
            return TService{};
        }
    };

    template<typename TService, typename... TDependencies>
    struct ServiceInstantiator<std::shared_ptr<TService>, TDependencies...>
    {
        template<typename TServiceContainer>
        static std::shared_ptr<TService> getInstance(TServiceContainer& services)
        {
            return std::make_shared<TService>(services.template get<TDependencies>()...);
        }
    };

    template<typename TService>
    struct ServiceInstantiator<std::shared_ptr<TService>>
    {
        template<typename TServiceContainer>
        static std::shared_ptr<TService> getInstance(TServiceContainer& services)
        {
            return std::make_shared<TService>();
        }
    };

    template<typename TService, typename TServiceInstantiator = ServiceInstantiator<TService>, typename TServiceFacade = TService>
    struct ServiceDependency
    {
        using ServiceType = TService;
        using ServiceFacadeType = TServiceFacade;
        using ServiceInstantiatorType = TServiceInstantiator;
    };

    template<typename TService, typename TServiceInstantiator, typename TServiceFacade>
    struct ServiceDependency<std::shared_ptr<TService>, TServiceInstantiator, TServiceFacade>
    {
        using ServiceType = std::shared_ptr<TService>;
        using ServiceFacadeType = TServiceFacade;
        using ServiceInstantiatorType = TServiceInstantiator;
    };

    template<typename TService, typename TServiceInstantiator = ServiceInstantiator<TService>, typename TServiceFacade = TService>
    struct TransientServiceDependency: ServiceDependency<TServiceFacade, TService, TServiceInstantiator>
    {
    };

    template<typename TDependency, typename TServiceContainer>
    struct DependencyController
    {
        TDependency::ServiceType service;

        explicit DependencyController(TServiceContainer& services):
                service(TDependency::ServiceInstantiatorType::template getInstance<TServiceContainer>(services))
        {}

    protected:
        TDependency::ServiceFacadeType& getInstance(TServiceContainer& services)
        {
            return service;
        }
    };

    template<typename TService, typename TServiceInstantiator, typename TServiceFacade, typename TServiceContainer>
    struct DependencyController<ServiceDependency<std::shared_ptr<TService>, TServiceInstantiator, TServiceFacade>, TServiceContainer>
    {
        using DependencyServiceType = std::shared_ptr<TService>;
        using DependencyServiceInstantiatorType = TServiceInstantiator;

        DependencyServiceType service;

        explicit DependencyController(TServiceContainer& services):
                service(DependencyServiceInstantiatorType::template getInstance<TServiceContainer>(services))
        {}

    protected:
        auto getInstance(TServiceContainer& services)
        {
            return std::static_pointer_cast<TServiceFacade>(service);
        }
    };

    template<typename TService, typename TServiceInstantiator, typename TServiceFacade, typename TServiceContainer>
    struct DependencyController<TransientServiceDependency<TService, TServiceInstantiator, TServiceFacade>, TServiceContainer>
    {
        explicit DependencyController(TServiceContainer& services)
        {}

    protected:
        decltype(auto) getInstance(TServiceContainer& services)
        {
            return TServiceInstantiator::template getInstance<TServiceContainer>(services);
        }
    };

    template<typename... TDependencies>
    struct ServiceContainer: public DependencyController<TDependencies, ServiceContainer<TDependencies...>>...
    {
        using ThisType = ServiceContainer<TDependencies...>;

        ServiceContainer():
                DependencyController<TDependencies, ThisType>(*this)...
        {}

        ServiceContainer(const ServiceContainer&) = delete;
        ServiceContainer& operator=(const ServiceContainer&) = delete;

        template<typename TDependency>
        decltype(auto) get()
        {
            return DependencyController<TDependency, ThisType>::getInstance(*this);
        }
    };
}
