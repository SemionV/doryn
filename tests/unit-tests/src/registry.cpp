#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/generic/typeList.h>
#include <array>
#include <dory/generic/extension/resourceHandle.h>

class ServiceInterface
{};

class IRenderService: public ServiceInterface
{};

class IResourceService: public ServiceInterface
{};

class IAudioService: public ServiceInterface
{};

using ServiceList = dory::generic::TypeList<
        IRenderService,
        IResourceService,
        IAudioService
    >;

using IdentifierList = dory::generic::ValueList<int, 1, 2, 3, 4, 5>;

enum class ServiceIdentifier
{
    Default
};

template<typename TServiceInterface, typename TIdentifierList = dory::generic::ValueList<ServiceIdentifier, ServiceIdentifier::Default>>
struct ServiceListEntry
{
    using InterfaceType = TServiceInterface;
    using IdentifierListType = TIdentifierList;
};

template<typename... TServices>
struct ServiceCountTraverse;

template<typename TService, typename... TServices>
struct ServiceCountTraverse<TService, TServices...>
{
    static constexpr std::size_t value = dory::generic::ValueListLength<typename TService::IdentifierListType>::value + ServiceCountTraverse<TServices...>::value;
};

template<>
struct ServiceCountTraverse<>
{
    static constexpr std::size_t value = 0;
};

template<typename TServiceList>
struct ServiceCount;

template<typename... TServices>
struct ServiceCount<dory::generic::TypeList<TServices...>>
{
    static constexpr std::size_t value = ServiceCountTraverse<TServices...>::value;
};

template<typename TServiceList>
class Registry
{
private:
    std::array<dory::generic::extension::ResourceHandle<std::shared_ptr<ServiceInterface>>, ServiceCount<TServiceList>::value> _services;

public:
    std::size_t getServiceSlotCount()
    {
        return _services.size();
    }
};

TEST(GenericTests, typeList)
{
    std::cout << dory::generic::TypeIndex<IResourceService, ServiceList>::value << std::endl;
    std::cout << dory::generic::ValueIndex<3, IdentifierList>::value << std::endl;

    using ServiceListLocal = dory::generic::TypeList<
            ServiceListEntry<IRenderService>,
            ServiceListEntry<IResourceService, dory::generic::ValueList<int, 1, 2, 3>>,
            ServiceListEntry<IAudioService, dory::generic::ValueList<int, 1, 2>>
        >;

    auto registry = Registry<ServiceListLocal>{};

    EXPECT_EQ(registry.getServiceSlotCount(), 6);
}