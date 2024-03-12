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

template<typename TService, typename... TDependencies>
struct ServiceInstantiator<std::shared_ptr<TService>, TDependencies...>
{
    template<typename TServiceLocator>
    static std::shared_ptr<TService> getInsatnce(TServiceLocator& services)
    {
        return std::make_shared<TService>((services.template get<TDependencies>(), ...));
    }
};

template<typename TService>
struct ServiceInstantiator<std::shared_ptr<TService>>
{
    template<typename TServiceLocator>
    static std::shared_ptr<TService> getInsatnce(TServiceLocator& services)
    {
        return std::make_shared<TService>();
    }
};

template<typename TService, typename TServiceInstantiator = ServiceInstantiator<TService>>
struct ServiceDependency
{
    using ServiceType = TService;
    using ServiceInstantiatorType = TServiceInstantiator;
};

template<typename TService, typename TServiceInstantiator>
struct ServiceDependency<std::shared_ptr<TService>, TServiceInstantiator>
{
    using ServiceType = std::shared_ptr<TService>;
    using ServiceInstantiatorType = TServiceInstantiator;
};

template<typename TService, typename TServiceInstantiator = ServiceInstantiator<TService>>
struct TransientServiceDependency: ServiceDependency<TService, TServiceInstantiator>
{
};

template<typename TDependency, typename TServiceLocator>
struct DependencyController
{
    TDependency::ServiceType service;

    explicit DependencyController(TServiceLocator& services):
            service(TDependency::ServiceInstantiatorType::template getInsatnce<TServiceLocator>(services))
    {}

protected:
    decltype(auto) getInstance(TServiceLocator& services)
    {
        return &service;
    }
};

template<typename TService, typename TServiceInstantiator, typename TServiceLocator>
struct DependencyController<ServiceDependency<std::shared_ptr<TService>, TServiceInstantiator>, TServiceLocator>
{
    using DependencyServiceType = ServiceDependency<std::shared_ptr<TService>, TServiceInstantiator>::ServiceType;
    using DependencyServiceInstantiatorType = ServiceDependency<TService, TServiceInstantiator>::ServiceInstantiatorType;

    DependencyServiceType service;

    explicit DependencyController(TServiceLocator& services):
            service(DependencyServiceInstantiatorType::template getInsatnce<TServiceLocator>(services))
    {}

protected:
    decltype(auto) getInstance(TServiceLocator& services)
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
    decltype(auto) getInstance(TServiceLocator& services)
    {
        return TServiceInstantiator::template getInsatnce<TServiceLocator>(services);
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
using Service2TransientDependency = TransientServiceDependency<Service1, ServiceInstantiator<Service2, Service1Dependency>>;
using Service2Dependency = ServiceDependency<Service2, ServiceInstantiator<Service2, Service1Dependency>>;
using Service2PointerDependency = ServiceDependency<std::shared_ptr<Service2>, ServiceInstantiator<std::shared_ptr<Service2>, Service1Dependency>>;

TEST_CASE("Check concept", "Service Locator")
{
    auto services = ServiceLocator<Service1Dependency,
                                        Service2Dependency,
                                        Service2PointerDependency,
                                        Service2TransientDependency>{};

    auto service1 = services.get<Service1Dependency>();
    if(service1)
    {
        std::cout << "Service1 value: " << service1->value << "\n";
    }

    auto service2 = services.get<Service2Dependency>();
    if(service2)
    {
        std::cout << "Service2 value: " << service2->value << "\n";
        std::cout << "Service2->service1 value: " << service2->service1->value << "\n";
    }

    auto service2Pointer = services.get<Service2PointerDependency>();
    if(service2Pointer)
    {
        service2Pointer->value = 3;
    }

    service2Pointer = services.get<Service2PointerDependency>();
    if(service2Pointer)
    {
        std::cout << "Service2Pointer value: " << service2Pointer->value << "\n";
        std::cout << "Service2Pointer->service1 value: " << service2Pointer->service1->value << "\n";
    }

    auto service2Transient = services.get<Service2TransientDependency>();
    REQUIRE(service2Transient.value == 2);
    service2Transient.value = 4;

    service2Transient = services.get<Service2TransientDependency>();
    REQUIRE(service2Transient.value == 2);
}