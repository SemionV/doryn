#include "dependencies.h"

template<typename TService, typename... TDependencies>
struct ServiceInstantiator
{
    template<typename TServiceLocator>
    static TService getInstance(TServiceLocator& services)
    {
        return TService(services.template get<TDependencies>()...);
    }
};

template<typename TService>
struct ServiceInstantiator<TService>
{
    template<typename TServiceLocator>
    static TService getInstance(TServiceLocator& services)
    {
        return TService{};
    }
};

template<typename TService, typename... TDependencies>
struct ServiceInstantiator<std::shared_ptr<TService>, TDependencies...>
{
    template<typename TServiceLocator>
    static std::shared_ptr<TService> getInstance(TServiceLocator& services)
    {
        return std::make_shared<TService>(services.template get<TDependencies>()...);
    }
};

template<typename TService>
struct ServiceInstantiator<std::shared_ptr<TService>>
{
    template<typename TServiceLocator>
    static std::shared_ptr<TService> getInstance(TServiceLocator& services)
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

template<typename TDependency, typename TServiceLocator>
struct DependencyController
{
    TDependency::ServiceType service;

    explicit DependencyController(TServiceLocator& services):
            service(TDependency::ServiceInstantiatorType::template getInstance<TServiceLocator>(services))
    {}

protected:
    TDependency::ServiceFacadeType& getInstance(TServiceLocator& services)
    {
        return static_cast<typename TDependency::ServiceFacadeType&>(service);
    }
};

template<typename TService, typename TServiceInstantiator, typename TServiceFacade, typename TServiceLocator>
struct DependencyController<ServiceDependency<std::shared_ptr<TService>, TServiceInstantiator, TServiceFacade>, TServiceLocator>
{
    using DependencyServiceType = std::shared_ptr<TService>;
    using DependencyServiceInstantiatorType = TServiceInstantiator;

    DependencyServiceType service;

    explicit DependencyController(TServiceLocator& services):
            service(DependencyServiceInstantiatorType::template getInstance<TServiceLocator>(services))
    {}

protected:
    auto& getInstance(TServiceLocator& services)
    {
        return *service;
    }
};

template<typename TService, typename TServiceInstantiator, typename TServiceFacade, typename TServiceLocator>
struct DependencyController<TransientServiceDependency<TService, TServiceInstantiator, TServiceFacade>, TServiceLocator>
{
    explicit DependencyController(TServiceLocator& services)
    {}

protected:
    decltype(auto) getInstance(TServiceLocator& services)
    {
        return TServiceInstantiator::template getInstance<TServiceLocator>(services);
    }
};

template<typename... TDependencies>
struct ServiceLocator: public DependencyController<TDependencies, ServiceLocator<TDependencies...>>...
{
    using ThisType = ServiceLocator<TDependencies...>;

    ServiceLocator():
            DependencyController<TDependencies, ThisType>(*this)...
    {}

    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    template<typename TDependency>
    decltype(auto) get()
    {
        return DependencyController<TDependency, ThisType>::getInstance(*this);
    }
};

/*----------------------------------------------------------------------------------------------------------------*/

class Service0
{
};

class Service1
{
public:
    int value = 1;
};

class Service2
{
public:
    Service1& service1;

public:
    explicit Service2(Service1& service1):
            service1(service1)
    {}

    int value = 2;
};

using Service1Dependency = ServiceDependency<Service1>;
using Service2TransientDependency = TransientServiceDependency<Service2, ServiceInstantiator<Service2, Service1Dependency>>;
using Service2Dependency = ServiceDependency<Service2, ServiceInstantiator<Service2, Service1Dependency>>;
using Service2PointerDependency = ServiceDependency<std::shared_ptr<Service2>, ServiceInstantiator<std::shared_ptr<Service2>, Service1Dependency>>;
using Service2TransientPointerDependency = TransientServiceDependency<std::shared_ptr<Service2>, ServiceInstantiator<std::shared_ptr<Service2>, Service1Dependency>>;

using ServiceLocatorType = ServiceLocator<Service1Dependency,
        Service2Dependency,
        Service2PointerDependency,
        Service2TransientDependency,
        Service2TransientPointerDependency>;

TEST_CASE("Check concept", "Service Locator")
{
    auto services = ServiceLocatorType{};

    auto service1 = services.get<Service1Dependency>();
    REQUIRE(service1.value == 1);

    auto service2 = services.get<Service2Dependency>();
    REQUIRE(service2.value == 2);

    auto service2Pointer = services.get<Service2PointerDependency>();
    REQUIRE(service2Pointer.value == 2);
    REQUIRE(service2Pointer.service1.value == 1);

    auto service2Transient = services.get<Service2TransientDependency>();
    REQUIRE(service2Transient.value == 2);
    service2Transient.value = 4;

    auto service2Transient2 {services.get<Service2TransientDependency>()};
    REQUIRE(service2Transient2.value == 2);

    auto service2TransientPointer = services.get<Service2TransientPointerDependency>();
    REQUIRE(service2TransientPointer->value == 2);
    service2TransientPointer->value = 4;

    service2TransientPointer = services.get<Service2TransientPointerDependency>();
    REQUIRE(service2TransientPointer->value == 2);
}

template<typename TImplementation>
class IPipelineService
{
public:
    std::vector<int> getPipeline()
    {
        return static_cast<TImplementation*>(this)->getPipelineImpl();
    }
};

class PipelineService: public IPipelineService<PipelineService>
{
private:
    std::vector<int> pipeline = {1};

public:
    auto getPipelineImpl()
    {
        std::cout << "PipelineService impl" << "\n";
        return pipeline;
    }
};

class PipelineService2: public IPipelineService<PipelineService2>
{
private:
    std::vector<int> pipeline = {2};

public:
    auto getPipelineImpl()
    {
        std::cout << "PipelineService2 impl" << "\n";
        return pipeline;
    }
};

template<typename TImplementation>
class IHelloService
{
public:
    void sayHello()
    {
        return static_cast<TImplementation*>(this)->sayHelloImpl();
    }
};

class HelloService: public IHelloService<HelloService>
{
public:
    void sayHelloImpl()
    {
        std::cout << "Hello!" << "\n";
    }
};

template<typename TPipelineService, typename THelloService>
class EngineService
{
private:
    IPipelineService<TPipelineService>& pipelineService;
    IHelloService<THelloService>& helloService;

public:
    explicit EngineService(IPipelineService<TPipelineService>& pipelineService,
                           IHelloService<THelloService>& helloService):
            pipelineService(pipelineService),
            helloService(helloService)
    {}

    void run()
    {
        auto pipeline = pipelineService.getPipeline();
        std::cout << pipeline.at(0) << "\n";

        helloService.sayHello();
    }
};

using PipelineServiceType = PipelineService2;

using PipelineDependency = ServiceDependency<PipelineServiceType, ServiceInstantiator<PipelineServiceType>, IPipelineService<PipelineServiceType>>;
using HelloServiceDependency = ServiceDependency<std::shared_ptr<HelloService>, ServiceInstantiator<std::shared_ptr<HelloService>>, IHelloService<HelloService>>;
using EngineDependency = ServiceDependency<EngineService<PipelineServiceType, HelloService>,
    ServiceInstantiator<EngineService<PipelineServiceType, HelloService>,
        PipelineDependency,
        HelloServiceDependency>>;

using ProjectServiceLocatorType = ServiceLocator<PipelineDependency,
        HelloServiceDependency,
        EngineDependency>;

TEST_CASE("Check ServiceLocator usage", "Service Locator")
{
    auto services = ProjectServiceLocatorType{};

    services.get<PipelineDependency>().getPipeline();

    auto engine = services.get<EngineDependency>();
    engine.run();
}