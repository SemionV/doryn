#include "base/serviceContainer.h"
#include "base/typeComponents.h"

using namespace dory;

class Service1
{
public:
    int value = 1;
};

class Service1Uncopiable: Uncopyable, public Service1
{};

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

class Service2Uncopiable: Uncopyable, public Service2
{
public:
    explicit Service2Uncopiable(Service1& service1):
            Service2(service1)
    {}
};

using Service1Dependency = Singleton<Service1Uncopiable>;
using Service2TransientDependency = Transient<Service2, Service2, Service1Dependency>;
using Service2Dependency = Singleton<Service2Uncopiable, Service2Uncopiable, Service1Dependency>;
using Service2PointerDependency = Singleton<std::shared_ptr<Service2Uncopiable>, Service2Uncopiable, Service1Dependency>;
using Service2TransientPointerDependency = Transient<std::shared_ptr<Service2>, Service2, Service1Dependency>;

using ServiceLocatorType = ServiceContainer<Service1Dependency,
        Service2Dependency,
        Service2PointerDependency,
        Service2TransientDependency,
        Service2TransientPointerDependency>;

TEST_CASE("Check concept", "Service Locator")
{
    auto services = ServiceLocatorType{};

    auto& service1 = services.get<Service1Dependency>();
    REQUIRE(service1.value == 1);
    service1.value = 2;

    auto& service2 = services.get<Service2Dependency>();
    REQUIRE(service2.value == 2);

    auto service2Pointer = services.get<Service2PointerDependency>();
    REQUIRE(service2Pointer->value == 2);
    REQUIRE(service2Pointer->service1.value == 2);

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
class IPipelineService: Uncopyable
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
    explicit PipelineService(int pipeline):
            pipeline(pipeline)
    {}

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
class IHelloService: Uncopyable
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
class EngineService: public Uncopyable
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
        REQUIRE(pipeline == -1);

        REQUIRE(helloService);
        REQUIRE(helloService->sayHello() == 3);
    }
};

struct ServiceDependencies
{
    using PipelineServiceType = PipelineService;
    using HelloServiceType = HelloService;
    using EngineServiceType = EngineService<PipelineServiceType, HelloServiceType>;

    using PipelineService = Singleton<PipelineServiceType, IPipelineService<PipelineServiceType>>;
    using HelloService = Singleton<std::shared_ptr<HelloServiceType>, IHelloService<HelloServiceType>>;
    using EngineService = Singleton<EngineServiceType, EngineServiceType, PipelineService, HelloService>;

    using ServiceContainerType = ServiceContainer<PipelineService,
            HelloService,
            EngineService>;
};

namespace dory
{
    /*custom instance factory*/
    template<>
    struct ServiceInstantiator<PipelineService>
    {
        template<typename TServiceContainer>
        static decltype(auto) createInstance(TServiceContainer& services)
        {
            return PipelineService{-1};
        }
    };
}

TEST_CASE("Check ServiceContainer usage", "Service Locator")
{
    using Deps = ServiceDependencies;

    auto services = Deps::ServiceContainerType{};

    services.get<Deps::PipelineService>().getPipeline();

    auto& engine = services.get<Deps::EngineService>();
    engine.run();
    engine.value = 6;

    REQUIRE(services.get<Deps::EngineService>().value == 6);
}