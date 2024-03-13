#include "base/serviceContainer.h"

using namespace dory;

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
using Service2PointerDependency = ServiceDependency<std::shared_ptr<Service2>, ServiceInstantiator<std::shared_ptr<Service2>, Service1Dependency>, Service2>;
using Service2TransientPointerDependency = TransientServiceDependency<std::shared_ptr<Service2>, ServiceInstantiator<std::shared_ptr<Service2>, Service1Dependency>>;

using ServiceLocatorType = ServiceContainer<Service1Dependency,
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
    REQUIRE(service2Pointer->value == 2);
    REQUIRE(service2Pointer->service1.value == 1);

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
    int getPipeline()
    {
        return static_cast<TImplementation*>(this)->getPipelineImpl();
    }
};

class PipelineService: public IPipelineService<PipelineService>
{
private:
    int pipeline = {1};

public:
    auto getPipelineImpl()
    {
        return pipeline;
    }
};

class PipelineService2: public IPipelineService<PipelineService2>
{
private:
    int pipeline = {2};

public:
    auto getPipelineImpl()
    {
        return pipeline;
    }
};

template<typename TImplementation>
class IHelloService
{
public:
    int sayHello()
    {
        return static_cast<TImplementation*>(this)->sayHelloImpl();
    }
};

class HelloService: public IHelloService<HelloService>
{
public:
    int sayHelloImpl()
    {
        return 3;
    }
};

template<typename TPipelineService, typename THelloService>
class EngineService
{
private:
    IPipelineService<TPipelineService>& pipelineService;
    std::shared_ptr<IHelloService<THelloService>> helloService;

public:
    explicit EngineService(IPipelineService<TPipelineService>& pipelineService,
                           std::shared_ptr<IHelloService<THelloService>> helloService):
            pipelineService(pipelineService),
            helloService(std::move(helloService))
    {}

    int value = 0;

    void run()
    {
        auto pipeline = pipelineService.getPipeline();
        REQUIRE(pipeline == 1);

        REQUIRE(helloService);
        REQUIRE(helloService->sayHello() == 3);
    }
};

struct ServiceDependencies
{
    using PipelineServiceType = PipelineService;
    using HelloServiceType = HelloService;
    using EngineServiceType = EngineService<PipelineServiceType, HelloServiceType>;

    using PipelineService = ServiceDependency<PipelineServiceType, ServiceInstantiator<PipelineServiceType>, IPipelineService<PipelineServiceType>>;
    using HelloService = ServiceDependency<std::shared_ptr<HelloServiceType>, ServiceInstantiator<std::shared_ptr<HelloServiceType>>, IHelloService<HelloServiceType>>;
    using EngineService = ServiceDependency<EngineServiceType,
            ServiceInstantiator<EngineServiceType,
                    PipelineService,
                    HelloService>>;

    using ServiceContainerType = ServiceContainer<PipelineService,
            HelloService,
            EngineService>;
};

TEST_CASE("Check ServiceContainer usage", "Service Locator")
{
    auto services = ServiceDependencies::ServiceContainerType{};

    services.get<ServiceDependencies::PipelineService>().getPipeline();

    auto& engine = services.get<ServiceDependencies::EngineService>();
    engine.run();
    engine.value = 6;

    REQUIRE(services.get<ServiceDependencies::EngineService>().value == 6);
}