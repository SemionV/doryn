#include <cassert>
#include "dependencies.h"
#include "typeComponents.h"

namespace dory
{
    template<typename TService, typename... TDependencies>
    struct ServiceInstantiator
    {
        template<typename TServiceContainer>
        static TService createInstance(TServiceContainer& services)
        {
            return TService(services.template get<TDependencies>()...);
        }
    };

    template<typename TService>
    struct ServiceInstantiator<TService>
    {
        template<typename TServiceContainer>
        static TService createInstance(TServiceContainer& services)
        {
            return TService{};
        }
    };

    template<typename TService, typename... TDependencies>
    struct ServiceInstantiator<std::shared_ptr<TService>, TDependencies...>
    {
        template<typename TServiceContainer>
        static std::shared_ptr<TService> createInstance(TServiceContainer& services)
        {
            return std::make_shared<TService>(services.template get<TDependencies>()...);
        }
    };

    template<typename TService>
    struct ServiceInstantiator<std::shared_ptr<TService>>
    {
        template<typename TServiceContainer>
        static std::shared_ptr<TService> createInstance(TServiceContainer& services)
        {
            return std::make_shared<TService>();
        }
    };

    template<typename... TDependencies>
    struct DependencyList
    {};

    template<typename TService, typename TServiceFacade = TService, typename TDependencyList = DependencyList<>, typename TRegistrationTag = void>
    struct DependencyDescriptor;

    template<typename TService, typename TServiceFacade, typename... TDependencies, typename TRegistrationTag>
    struct DependencyDescriptor<TService, TServiceFacade, DependencyList<TDependencies...>, TRegistrationTag>
    {
        using ServiceType = TService;
        using ServiceFacadeType = TServiceFacade;

        template<typename TServiceContainer>
        static TService createInstance(TServiceContainer& services)
        {
            return ServiceInstantiator<TService, TDependencies...>::createInstance(services);
        }
    };

    template<typename TService, typename TServiceFacade = TService, typename TDependencyList = DependencyList<>, typename TRegistrationTag = void>
    requires(!std::is_copy_constructible_v<TServiceFacade>)
    struct Singleton: public DependencyDescriptor<TService, TServiceFacade, TDependencyList, TRegistrationTag>
    {
    };

    template<typename TDependency, typename TServiceFacade = TDependency::ServiceFacadeType, typename TRegistrationTag = void>
    requires(!std::is_copy_constructible_v<TServiceFacade>)
    struct Reference: public DependencyDescriptor<typename TDependency::ServiceType, TServiceFacade, DependencyList<>, TRegistrationTag>
    {
    };

    template<typename TService, typename TServiceFacade = TService, typename TDependencyList = DependencyList<>, typename TRegistrationTag = void>
    requires(std::is_copy_constructible_v<TServiceFacade>)
    struct Transient: public DependencyDescriptor<TService, TServiceFacade, TDependencyList, TRegistrationTag>
    {
    };

    template<typename TDependency, typename TServiceContainer>
    struct SingletonDependencyController
    {
        TDependency::ServiceType service;
        bool isInstantiated = false;

        explicit SingletonDependencyController(TServiceContainer& services):
            service(TDependency::createInstance(services))
        {
            isInstantiated = true;
        }

        void assertInstance()
        {
            if(!isInstantiated)
            {
                std::cout << "Dependency is required before it is instantiated: [" << typeid(typename TDependency::ServiceType).name() << "]" << "\n";
                assert(false);
            }
        }
    };

    template<typename TDependency, typename TServiceContainer, std::size_t DependencyId = 0>
    struct DependencyController: public SingletonDependencyController<TDependency, TServiceContainer>
    {
        explicit DependencyController(TServiceContainer& services):
                SingletonDependencyController<TDependency, TServiceContainer>(services)
        {}

        template<typename TServiceFacade = TDependency::ServiceFacadeType>
        TServiceFacade& getInstance(TServiceContainer& services)
        {
            SingletonDependencyController<TDependency, TServiceContainer>::assertInstance();
            return SingletonDependencyController<TDependency, TServiceContainer>::service;
        }
    };

    template<typename TService, typename TServiceFacade, typename TServiceContainer, typename... TDependencies>
    struct DependencyController<Singleton<std::shared_ptr<TService>, TServiceFacade, TDependencies...>, TServiceContainer>:
            public SingletonDependencyController<Singleton<std::shared_ptr<TService>, TServiceFacade, TDependencies...>, TServiceContainer>
    {
        using DependencyType = Singleton<std::shared_ptr<TService>, TServiceFacade, TDependencies...>;

        explicit DependencyController(TServiceContainer& services):
                SingletonDependencyController<DependencyType, TServiceContainer>(services)
        {}

    protected:
        template<typename TGetServiceFacade = TServiceFacade>
        auto getInstance(TServiceContainer& services)
        {
            SingletonDependencyController<DependencyType, TServiceContainer>::assertInstance();
            return std::static_pointer_cast<TGetServiceFacade>(SingletonDependencyController<DependencyType, TServiceContainer>::service);
        }
    };

    template<typename TService, typename TServiceFacade, typename TServiceContainer, typename... TDependencies>
    struct DependencyController<Transient<TService, TServiceFacade, TDependencies...>, TServiceContainer>
    {
        explicit DependencyController(TServiceContainer& services)
        {}

    protected:
        template<typename TGetServiceFacade = TServiceFacade>
        auto getInstance(TServiceContainer& services)
        {
            return static_cast<TGetServiceFacade>(Transient<TService, TServiceFacade, TDependencies...>::createInstance(services));
        }
    };

    template<typename TService, typename TServiceFacade, typename TServiceContainer, typename... TDependencies>
    struct DependencyController<Transient<std::shared_ptr<TService>, TServiceFacade, TDependencies...>, TServiceContainer>
    {
        explicit DependencyController(TServiceContainer& services)
        {}

    protected:
        template<typename TGetServiceFacade = TServiceFacade>
        decltype(auto) getInstance(TServiceContainer& services)
        {
            return std::static_pointer_cast<TGetServiceFacade>(Transient<std::shared_ptr<TService>, TServiceFacade, TDependencies...>::createInstance(services));
        }
    };

    template<typename TDependency, typename TServiceFacade, typename TServiceContainer>
    struct DependencyController<Reference<TDependency, TServiceFacade>, TServiceContainer>
    {
        explicit DependencyController(TServiceContainer& services)
        {}

    protected:
        template<typename TGetServiceFacade = TServiceFacade>
        decltype(auto) getInstance(TServiceContainer& services)
        {
            return services.template get<TDependency, TGetServiceFacade>();
        }
    };

    template<typename... TDependencies>
    struct ServiceContainer: Uncopyable, public DependencyController<TDependencies, ServiceContainer<TDependencies...>>...
    {
        using ThisType = ServiceContainer<TDependencies...>;

        ServiceContainer():
                DependencyController<TDependencies, ThisType>(*this)...
        {}

        template<typename TDependency, typename TServiceFacade = TDependency::ServiceFacadeType>
        decltype(auto) get()
        {
            return DependencyController<TDependency, ThisType>::template getInstance<TServiceFacade>(*this);
        }
    };
}
