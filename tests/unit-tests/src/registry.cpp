#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/generic/typeList.h>
#include <dory/generic/extension/libraryHandle.h>
#include <dory/macros/assert.h>
#include <dory/core/registry2.h>

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

using namespace dory::core;

TEST(GenericTests, typeList)
{
    using ServiceListLocal = dory::generic::TypeList<
            ServiceListEntry<IRenderService>,
            ServiceListEntry<IResourceService, dory::generic::ValueList<int, 1, 2, 3>>,
            ServiceListEntry<IAudioService, dory::generic::ValueList<int, 1, 2>>
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