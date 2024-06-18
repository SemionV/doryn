#include "dependencies.h"
#include "base/domain/services/moduleService.h"
#include "base/domain/events/event.h"

struct TestContext
{
    bool isEventHandled = false;
};

struct TestEvent
{};

using Hub = dory::domain::events::EventHub<TestContext, const TestEvent>;
using Dispatcher = dory::domain::events::EventCannon<Hub>;

struct TestServices
{
    bool isTestAttached = false;
    bool isTestDetached = false;
    Dispatcher dispatcher;
};

class TestModule: public dory::ILoadableModule<TestServices>
{
public:
    void attach(std::weak_ptr<dory::ILibrary> library, TestServices& registry) override
    {
        registry.isTestAttached = true;
        registry.dispatcher.attach(library, std::function{ [](TestContext& context, const TestEvent& event)
        {
            context.isEventHandled = true;
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

TEST_CASE( "Load module", "[modules]" )
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