#include "dependencies.h"

template<typename TService, typename... TDependencies>
struct ServiceInstantiator
{
    template<typename TServiceLocator>
    static TService getInsatnce(TServiceLocator& services)
    {
        return TService{(services.template get<TDependencies>(), ...)};
    }
};

template<typename TService>
struct ServiceInstantiator<TService>
{
    template<typename TServiceLocator>
    static TService getInsatnce(TServiceLocator& services)
    {
        return TService{};
    }
};

/*template<typename TService, typename... TDependencies>
struct ServiceInstantiator
{
    template<typename TServiceLocator>
    static TService getInsatnce(TServiceLocator& services)
    {
        return TService{(services.template get<TDependencies>(), ...)};
    }
};*/

template<typename TService, typename TServiceInstantiator = ServiceInstantiator<TService>>
struct ServiceDependency
{
    using ServiceType = TService;
    using ServiceValueType = TService;
    using ServiceInstantiatorType = TServiceInstantiator;
};

template<typename TService, typename TServiceInstantiator>
struct ServiceDependency<std::shared_ptr<TService>, TServiceInstantiator>
{
    using ServiceType = TService;
    using ServiceValueType = std::shared_ptr<TService>;
    using ServiceInstantiatorType = TServiceInstantiator;
};

template<typename TService, typename TServiceInstantiator>
struct TransientServiceDependency: ServiceDependency<TService, TServiceInstantiator>
{
};

template<typename TDependency, typename TServiceLocator>
struct DependencyController
{
    TDependency::ServiceValueType service;

    explicit DependencyController(TServiceLocator& services):
            service(TDependency::ServiceInstantiatorType::template getInsatnce<TServiceLocator>(services))
    {}

protected:
    [[nodiscard]] decltype(auto) getInstance(TServiceLocator& services)
    {
        return &service;
    }
};

template<typename TService, typename TServiceInstantiator, typename TServiceLocator>
struct DependencyController<ServiceDependency<std::shared_ptr<TService>, TServiceInstantiator>, TServiceLocator>
{
    using DependencyServiceValueType = ServiceDependency<std::shared_ptr<TService>, TServiceInstantiator>::ServiceValueType;
    using DependencyServiceInstantiatorType = ServiceDependency<TService, TServiceInstantiator>::ServiceInstantiatorType;

    DependencyServiceValueType service;

    explicit DependencyController(TServiceLocator& services):
            service(DependencyServiceInstantiatorType::template getInsatnce<TServiceLocator>(services))
    {}

protected:
    [[nodiscard]] decltype(auto) getInstance(TServiceLocator& services)
    {
        return service;
    }
};

template<typename TService, typename TServiceInstantiator, typename TServiceLocator>
struct DependencyController<TransientServiceDependency<TService, TServiceInstantiator>, TServiceLocator>
{
    explicit DependencyController(TServiceLocator& services)
    {}

protected:
    [[nodiscard]] decltype(auto) getInstance(TServiceLocator& services)
    {
        return TServiceInstantiator::template getInsatnce<TService, TServiceLocator>(services);
    }
};

template<typename... TDependencies>
struct ServiceLocator: public DependencyController<TDependencies, ServiceLocator<TDependencies...>>...
{
    ServiceLocator():
            DependencyController<TDependencies, ServiceLocator<TDependencies...>>(*this)...
    {}

    template<typename TDependency>
    decltype(auto) get()
    {
        return DependencyController<TDependency, ServiceLocator<TDependencies...>>::getInstance(*this);
    }
};

class Service1
{
public:
    int value = 1;
};

class Service2
{
public:
    Service1* service1;

public:
    explicit Service2(Service1* service1):
            service1(service1)
    {}

    int value = 2;
};

using Service1Dependency = ServiceDependency<Service1>;
using Service2Dependency = ServiceDependency<Service2, ServiceInstantiator<Service2, Service1Dependency>>;

template<typename TService>
struct Service2PointerInstantiator
{
    template<typename TServiceLocator>
    static decltype(auto) getInsatnce(TServiceLocator& services)
    {
        auto service1 = services.template get<Service1Dependency>();
        auto service = std::make_shared<TService>(service1);
        service->value = 3;
        return service;
    }
};
using Service2PointerDependency = ServiceDependency<std::shared_ptr<Service2>, Service2PointerInstantiator<Service2>>;

TEST_CASE("Check concept", "Service Locator")
{
    auto serviceLocator = ServiceLocator<Service1Dependency,
                                        Service2Dependency,
                                        Service2PointerDependency>{};

    auto service1 = serviceLocator.get<Service1Dependency>();
    if(service1)
    {
        std::cout << "Service1 value: " << service1->value << "\n";
    }

    auto service2 = serviceLocator.get<Service2Dependency>();
    if(service2)
    {
        std::cout << "Service2 value: " << service2->value << "\n";
        std::cout << "Service2->service1 value: " << service2->service1->value << "\n";
    }

    auto service2Pointer = serviceLocator.get<Service2PointerDependency>();
    if(service2Pointer)
    {
        std::cout << "Service2Pointer value: " << service2Pointer->value << "\n";
        std::cout << "Service2Pointer->service1 value: " << service2Pointer->service1->value << "\n";
    }
}