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

class IPhysicsService: public ServiceInterface
{};

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

template<std::size_t Index, typename TServiceInterface, typename... TServices>
struct ServiceIndexTraverse;

template<std::size_t Index, typename TServiceInterface, typename TService, typename... TServices>
struct ServiceIndexTraverse<Index, TServiceInterface, TService, TServices...>
{
    using NextServiceTraverseType = ServiceIndexTraverse<Index + dory::generic::ValueListLength<typename TService::IdentifierListType>::value, TServiceInterface, TServices...>;
    using ServiceEntryType = std::conditional_t<std::is_same_v<TServiceInterface, typename TService::InterfaceType>, TService, typename NextServiceTraverseType::ServiceEntryType>;
    static constexpr int value = std::is_same_v<TServiceInterface, typename TService::InterfaceType> ? Index : NextServiceTraverseType::value;
};

template<std::size_t Index, typename TServiceInterface>
struct ServiceIndexTraverse<Index, TServiceInterface>
{
    using ServiceEntryType = ServiceListEntry<void>;
    static constexpr int value = -1;
};

template<typename TServiceInterface, auto Identifier, typename TServiceList>
struct ServiceIndex;

template<typename TServiceInterface, auto Identifier, typename... TServices>
struct ServiceIndex<TServiceInterface, Identifier, dory::generic::TypeList<TServices...>>
{
    using IndexTraverseType = ServiceIndexTraverse<0, TServiceInterface, TServices...>;
    static constexpr int offset = dory::generic::ValueIndex<Identifier, typename IndexTraverseType::ServiceEntryType::IdentifierListType>::value;
    static_assert(offset >= 0); //Service Identifier is not registered
    static constexpr int value = IndexTraverseType::value + offset;
    static_assert(value >= 0); // Service Interface is not registered
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
    using ServiceListLocal = dory::generic::TypeList<
            ServiceListEntry<IRenderService>,
            ServiceListEntry<IResourceService, dory::generic::ValueList<int, 1, 2, 3>>,
            ServiceListEntry<IAudioService, dory::generic::ValueList<int, 1, 2>>
        >;

    auto registry = Registry<ServiceListLocal>{};

    EXPECT_EQ(registry.getServiceSlotCount(), 6);

    constexpr auto index1 = ServiceIndex<IResourceService, 2, ServiceListLocal>::value;
    EXPECT_EQ(index1, 2);

    constexpr auto index2 = ServiceIndex<IAudioService, 1, ServiceListLocal>::value;
    EXPECT_EQ(index2, 4);

    constexpr auto index3 = ServiceIndex<IRenderService, ServiceIdentifier::Default, ServiceListLocal>::value;
    EXPECT_EQ(index3, 0);
}