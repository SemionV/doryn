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

    template<typename TService, typename TServiceFacade = TService, typename... TDependencies>
    struct DependencyDescriptor
    {
        using ServiceType = TService;
        using ServiceFacadeType = TServiceFacade;

        template<typename TServiceContainer>
        static TService createInstance(TServiceContainer& services)
        {
            return ServiceInstantiator<TService, TDependencies...>::createInstance(services);
        }
    };

    template<typename TService, typename TServiceFacade = TService, typename... TDependencies>
    requires(!std::is_copy_constructible_v<TServiceFacade>)
    struct Singleton: public DependencyDescriptor<TService, TServiceFacade, TDependencies...>
    {
    };

    template<typename TDependency, typename TServiceFacade = TDependency::ServiceFacadeType>
    requires(!std::is_copy_constructible_v<TServiceFacade>)
    struct Reference: public DependencyDescriptor<typename TDependency::ServiceType, TServiceFacade>
    {
    };

    template<typename TService, typename TServiceFacade = TService, typename... TDependencies>
    requires(std::is_copy_constructible_v<TServiceFacade>)
    struct Transient: public DependencyDescriptor<TService, TServiceFacade, TDependencies...>
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

    template<typename TDependency, typename TServiceContainer>
    struct DependencyController: public SingletonDependencyController<TDependency, TServiceContainer>
    {
        explicit DependencyController(TServiceContainer& services):
                SingletonDependencyController<TDependency, TServiceContainer>(services)
        {}

        template<typename TServiceFacade = TDependency::ServiceFacadeType>
        auto& getInstance(TServiceContainer& services)
        {
            SingletonDependencyController<TDependency, TServiceContainer>::assertInstance();

            if constexpr (!std::is_same_v<TServiceFacade, typename TDependency::ServiceFacadeType>)
            {
                return static_cast<TServiceFacade&>(SingletonDependencyController<TDependency, TServiceContainer>::service);
            }
            return static_cast<TDependency::ServiceFacadeType&>(SingletonDependencyController<TDependency, TServiceContainer>::service);
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

            if constexpr (!std::is_same_v<TGetServiceFacade, TServiceFacade>)
            {
                return std::static_pointer_cast<TGetServiceFacade>(SingletonDependencyController<DependencyType, TServiceContainer>::service);
            }

            return std::static_pointer_cast<TServiceFacade>(SingletonDependencyController<DependencyType, TServiceContainer>::service);
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
            if constexpr (!std::is_same_v<TGetServiceFacade, TServiceFacade>)
            {
                return static_cast<TGetServiceFacade>(Transient<TService, TServiceFacade, TDependencies...>::createInstance(services));
            }
            return Transient<TService, TServiceFacade, TDependencies...>::createInstance(services);
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
            if constexpr (!std::is_same_v<TGetServiceFacade, TServiceFacade>)
            {
                return std::static_pointer_cast<TGetServiceFacade>(Transient<std::shared_ptr<TService>, TServiceFacade, TDependencies...>::createInstance(services));
            }

            return std::static_pointer_cast<TServiceFacade>(Transient<std::shared_ptr<TService>, TServiceFacade, TDependencies...>::createInstance(services));
        }
    };

    template<typename TDependency, typename TServiceFacade, typename TServiceContainer>
    struct DependencyController<Reference<TDependency, TServiceFacade>, TServiceContainer>
    {
        explicit DependencyController(TServiceContainer& services)
        {}

    protected:
        decltype(auto) getInstance(TServiceContainer& services)
        {
            return services.template get<TDependency, TServiceFacade>();
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
            return DependencyController<TDependency, ThisType>::getInstance(*this);
        }
    };
}
