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

    template<typename TDependency>
    struct Singleton
    {
        using Type = TDependency;
    };

    template<typename TDependency>
    struct Transient: Singleton<TDependency>
    {
    };

    template<typename TService, typename TServiceInstantiator = ServiceInstantiator<TService>, typename TServiceFacade = TService, typename... TDependencies>
    struct Dependency
    {
        using ServiceType = TService;
        using ServiceFacadeType = TServiceFacade;
        using ServiceInstantiatorType = TServiceInstantiator;

        template<typename TServiceContainer>
        static ServiceType createInstance(TServiceContainer& services)
        {
            return ServiceType(services.template get<TDependencies>()...);
        }
    };

    template<typename TService, typename TServiceInstantiator, typename TServiceFacade, typename... TDependencies>
    struct Dependency<std::shared_ptr<TService>, TServiceInstantiator, TServiceFacade, TDependencies...>
    {
        using ServiceType = std::shared_ptr<TService>;
        using ServiceFacadeType = TServiceFacade;
        using ServiceInstantiatorType = TServiceInstantiator;

        template<typename TServiceContainer>
        static ServiceType createInstance(TServiceContainer& services)
        {
            return std::make_shared<TService>(services.template get<TDependencies>()...);
        }
    };

    template<typename TService, typename TServiceInstantiator, typename TServiceFacade>
    struct Dependency<std::shared_ptr<TService>, TServiceInstantiator, TServiceFacade>
    {
        using ServiceType = TService;
        using ServiceFacadeType = TServiceFacade;
        using ServiceInstantiatorType = TServiceInstantiator;

        template<typename TServiceContainer>
        static ServiceType createInstance(TServiceContainer& services)
        {
            return ServiceType{};
        }
    };

    template<typename TService, typename TServiceInstantiator = ServiceInstantiator<TService>, typename TServiceFacade = TService, typename... TDependencies>
    struct TransientDependency: Dependency<TService, TServiceInstantiator, TServiceFacade, TDependencies...>
    {
    };

    template<typename TDependency, typename TServiceContainer>
    struct DependencyController
    {
        TDependency::ServiceType service;

        explicit DependencyController(TServiceContainer& services):
                service(TDependency::template createInstance<TServiceContainer>(services))
        {}

    protected:
        TDependency::ServiceFacadeType& getInstance(TServiceContainer& services)
        {
            return service;
        }
    };

    template<typename TService, typename TServiceInstantiator, typename TServiceFacade, typename TServiceContainer>
    struct DependencyController<Dependency<std::shared_ptr<TService>, TServiceInstantiator, TServiceFacade>, TServiceContainer>
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

    template<typename TService, typename TServiceInstantiator, typename TServiceFacade, typename TServiceContainer, typename... TDependencies>
    struct DependencyController<TransientDependency<TService, TServiceInstantiator, TServiceFacade, TDependencies...>, TServiceContainer>
    {
        explicit DependencyController(TServiceContainer& services)
        {}

    protected:
        decltype(auto) getInstance(TServiceContainer& services)
        {
            return TransientDependency<TService, TServiceInstantiator, TServiceFacade, TDependencies...>::template createInstance<TServiceContainer>(services);
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
