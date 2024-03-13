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
    struct DependencyDescriptor
    {
        using ServiceType = TService;
        using ServiceFacadeType = TServiceFacade;
        using ServiceInstantiatorType = TServiceInstantiator;
    };

    template<typename TService, typename TServiceInstantiator = ServiceInstantiator<TService>, typename TServiceFacade = TService, typename... TDependencies>
    struct Singleton: DependencyDescriptor<TService, TServiceInstantiator, TServiceFacade>
    {
        template<typename TServiceContainer>
        static TService createInstance(TServiceContainer& services)
        {
            return TService(services.template get<TDependencies>()...);
        }
    };

    template<typename TService, typename TServiceInstantiator, typename TServiceFacade, typename... TDependencies>
    struct Singleton<std::shared_ptr<TService>, TServiceInstantiator, TServiceFacade, TDependencies...>: DependencyDescriptor<std::shared_ptr<TService>, TServiceInstantiator, TServiceFacade>
    {
        template<typename TServiceContainer>
        static std::shared_ptr<TService> createInstance(TServiceContainer& services)
        {
            return std::make_shared<TService>(services.template get<TDependencies>()...);
        }
    };

    template<typename TService, typename TServiceInstantiator, typename TServiceFacade>
    struct Singleton<std::shared_ptr<TService>, TServiceInstantiator, TServiceFacade>: DependencyDescriptor<std::shared_ptr<TService>, TServiceInstantiator, TServiceFacade>
    {
        template<typename TServiceContainer>
        static std::shared_ptr<TService> createInstance(TServiceContainer& services)
        {
            return std::make_shared<TService>();
        }
    };

    template<typename TService, typename TServiceInstantiator, typename TServiceFacade>
    struct Singleton<TService, TServiceInstantiator, TServiceFacade>: DependencyDescriptor<TService, TServiceInstantiator, TServiceFacade>
    {
        template<typename TServiceContainer>
        static TService createInstance(TServiceContainer& services)
        {
            return TService{};
        }
    };

    template<typename TService, typename TServiceInstantiator = ServiceInstantiator<TService>, typename TServiceFacade = TService, typename... TDependencies>
    struct Transient: Singleton<TService, TServiceInstantiator, TServiceFacade, TDependencies...>
    {
    };

    template<typename TDependency, typename TServiceContainer>
    struct DependencyController
    {
        TDependency::ServiceType service;

        explicit DependencyController(TServiceContainer& services):
                service(TDependency::createInstance(services))
        {}

    protected:
        TDependency::ServiceFacadeType& getInstance(TServiceContainer& services)
        {
            return service;
        }
    };

    template<typename TService, typename TServiceInstantiator, typename TServiceFacade, typename TServiceContainer, typename... TDependencies>
    struct DependencyController<Singleton<std::shared_ptr<TService>, TServiceInstantiator, TServiceFacade, TDependencies...>, TServiceContainer>
    {
        using DependencyServiceType = std::shared_ptr<TService>;

        DependencyServiceType service;

        explicit DependencyController(TServiceContainer& services):
                service(Singleton<std::shared_ptr<TService>, TServiceInstantiator, TServiceFacade, TDependencies...>::createInstance(services))
        {}

    protected:
        auto getInstance(TServiceContainer& services)
        {
            return std::static_pointer_cast<TServiceFacade>(service);
        }
    };

    template<typename TService, typename TServiceInstantiator, typename TServiceFacade, typename TServiceContainer, typename... TDependencies>
    struct DependencyController<Transient<TService, TServiceInstantiator, TServiceFacade, TDependencies...>, TServiceContainer>
    {
        explicit DependencyController(TServiceContainer& services)
        {}

    protected:
        decltype(auto) getInstance(TServiceContainer& services)
        {
            return Transient<TService, TServiceInstantiator, TServiceFacade, TDependencies...>::createInstance(services);
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
