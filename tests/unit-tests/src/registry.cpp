#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/generic/typeList.h>
#include <dory/generic/extension/libraryHandle.h>
#include <dory/macros/assert.h>
#include <dory/generic/registryLayer.h>

class IRenderService
{};

class RenderService: public IRenderService
{
};

class IResourceService
{};

class ResourceService: public IResourceService
{
};

class IAudioService
{};

class IPhysicsService
{};

using namespace dory::generic::registry;

TEST(GenericTests, typeList)
{
    using ServiceListLocal = dory::generic::TypeList<
            ServiceEntry<IRenderService>,
            ServiceEntry<IResourceService, dory::generic::ValueList<int, 1, 2, 3>>,
            ServiceEntry<IAudioService, dory::generic::ValueList<int, 1, 2>>
        >;

    auto registry = RegistryLayer<ServiceListLocal>{};

    EXPECT_EQ(registry.getServiceSlotCount(), 6);

    constexpr auto index1 = ServiceIndex<IResourceService, 2, ServiceListLocal>::value;
    EXPECT_EQ(index1, 2);

    constexpr auto index2 = ServiceIndex<IAudioService, 1, ServiceListLocal>::value;
    EXPECT_EQ(index2, 4);

    constexpr auto index3 = ServiceIndex<IRenderService, ServiceIdentifier::Default, ServiceListLocal>::value;
    EXPECT_EQ(index3, 0);

    const auto renderService = std::make_shared<RenderService>();
    registry.set<IRenderService>(dory::generic::extension::LibraryHandle{}, renderService);

    auto renderServiceRef = registry.get<IRenderService>();
    EXPECT_TRUE(renderServiceRef.operator bool());

    const auto resourceService = std::make_shared<ResourceService>();
    registry.set<IResourceService, 2>(dory::generic::extension::LibraryHandle{}, resourceService);

    auto resourceServiceRef = registry.get<IResourceService, 2>();
    EXPECT_TRUE(resourceServiceRef.operator bool());

    auto resourceServiceRef2 = registry.get<IResourceService>(2);
    EXPECT_TRUE(resourceServiceRef2.operator bool());

    registry.getAll<IResourceService>([](const int identifier, IResourceService* service)
    {
        std::cout << "id: " << identifier << std::endl;
    });
}

class IListener
{
public:
    virtual ~IListener() = default;

    virtual void attach() = 0;
};

class IDispatcher
{
public:
    virtual ~IDispatcher() = default;

    virtual void fire() = 0;
};

class Dispatcher: public IDispatcher
{
public:
    void fire() override
    {
        std::cout << "fire" << std::endl;
    }
};

class Listener: public IListener, public Dispatcher
{
public:
    void attach() override
    {
        std::cout << "attach" << std::endl;
    }
};

TEST(TypeEarasure, test)
{
    auto ptr = std::make_shared<Listener>();

    auto vptr = std::static_pointer_cast<void>(ptr);

    auto listener = std::static_pointer_cast<IListener>(vptr);
    listener->attach();

    auto dispatcher = std::static_pointer_cast<IDispatcher>(vptr);
    dispatcher->fire();
}