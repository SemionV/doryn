#include "dependencies.h"
#include "base/domain/services/moduleService.h"

struct TestServices
{
    bool isTestAttached = false;
    bool isTestDetached = false;
};

class TestModule: public dory::ILoadableModule<TestServices>
{
public:
    void attach(std::shared_ptr<dory::ILibrary> library, TestServices& registry) override
    {
        registry.isTestAttached = true;
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
            return _module;
        }

        return nullptr;
    }
};

TEST_CASE( "Load module", "[modules]" )
{
    auto logger = dory::domain::services::LogServiceNull{};
    auto moduleService  = dory::domain::services::module::ModuleService<TestServices, decltype(logger), TestDynamicLinkLibrary>{ logger };

    auto services = TestServices{};

    moduleService.load("test-module", "test", services);
    moduleService.unload("test-module", services);

    REQUIRE(services.isTestAttached);
    REQUIRE(services.isTestDetached);
}