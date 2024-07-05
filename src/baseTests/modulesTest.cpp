#include "dependencies.h"

struct TestContext
{
    bool isEventHandled = false;
};

struct TestEvent {};
struct LockEvent
{
    std::shared_ptr<dory::ILibrary> lock;
};

using Hub = dory::domain::events::EventHub<TestContext, const TestEvent, LockEvent>;
using Dispatcher = dory::domain::events::EventCannon<Hub>;

struct TestServices
{
    bool isTestAttached = false;
    bool isTestDetached = false;
    Dispatcher dispatcher;
};

class TestModule: public dory::ILoadableModule<TestServices>
{
private:
    std::weak_ptr<dory::ILibrary> _library;

public:
    void attach(std::weak_ptr<dory::ILibrary> library, TestServices& registry) override
    {
        _library = library;

        registry.isTestAttached = true;
        registry.dispatcher.attach(library, std::function{ [](TestContext& context, const TestEvent& event)
        {
            context.isEventHandled = true;
        }});

        registry.dispatcher.attach(library, std::function{ [this](TestContext& context, LockEvent& event)
        {
            event.lock = _library.lock();
        }});
    }

    void detach(TestServices& registry) override
    {
        registry.isTestDetached = true;
    }
};

class TestDynamicLinkLibrary: public dory::DynamicLinkLibrary<TestServices>
{
private:
    bool _isLoaded = false;
    std::shared_ptr<TestModule> _module;

public:
    bool isLoaded() override
    {
        return _isLoaded;
    }

    std::shared_ptr<dory::ILoadableModule<TestServices>> unload() override
    {
        _isLoaded = false;
        auto tmpModule = _module;
        _module = nullptr;

        return tmpModule;
    }

    std::shared_ptr<dory::ILoadableModule<TestServices>> load(const std::filesystem::path& libraryPath) override
    {
        if(libraryPath == "test")
        {
            _module = std::make_shared<TestModule>();
            _isLoaded = true;
            return _module;
        }

        return nullptr;
    }
};

TEST_CASE( "Load and unload module", "[modules]" )
{
    auto logger = dory::domain::services::LogServiceNull{};
    auto moduleService  = dory::domain::services::module::ModuleService<TestServices, decltype(logger), TestDynamicLinkLibrary>{ logger };

    auto context = TestContext{};
    auto services = TestServices{};

    moduleService.load("test-module", "test", services);

    services.dispatcher.fire(context, TestEvent{});
    REQUIRE(context.isEventHandled);

    moduleService.unload("test-module", services);

    context.isEventHandled = false;
    services.dispatcher.fire(context, TestEvent{});
    REQUIRE(!context.isEventHandled);

    REQUIRE(services.isTestAttached);
    REQUIRE(services.isTestDetached);
}

TEST_CASE( "Unload module with lock", "[modules]" )
{
    auto logger = dory::domain::services::LogServiceNull{};
    auto moduleService  = dory::domain::services::module::ModuleService<TestServices, decltype(logger), TestDynamicLinkLibrary>{ logger };

    auto context = TestContext{};
    auto services = TestServices{};

    moduleService.load("test-module", "test", services);

    auto lockEvent = LockEvent{};

    services.dispatcher.fire(context, lockEvent);
    REQUIRE(lockEvent.lock);

    moduleService.unload("test-module", services);

    services.dispatcher.fire(context, TestEvent{});
    REQUIRE(!context.isEventHandled);
}