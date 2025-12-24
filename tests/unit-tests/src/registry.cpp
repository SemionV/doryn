#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/generic/typeList.h>
#include <array>
#include <dory/generic/extension/resourceHandle.h>
#include <dory/generic/extension/libraryHandle.h>

class IRenderService
{};

class RenderService: public IRenderService
{
};

class IResourceService
{};

class IAudioService
{};

class IPhysicsService
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
    using BaseInterfaceType = void;
    template<typename TInterface>
    using ServicePointerType = std::shared_ptr<TInterface>;
    using ServiceHandleType = dory::generic::extension::ResourceHandle<ServicePointerType<BaseInterfaceType>>;
    using StorageEntryType = std::optional<ServiceHandleType>;
    std::array<StorageEntryType, ServiceCount<TServiceList>::value> _services;

private:
    template<typename TServiceInterface>
    constexpr int getServiceEntryIndex()
    {
        constexpr auto index = ServiceIndex<TServiceInterface, ServiceIdentifier::Default, TServiceList>::value;
        static_assert(index >= 0 && index < dory::generic::Length<TServiceList>::value); //Index is out of range

        return index;
    }

public:
    std::size_t getServiceSlotCount()
    {
        return _services.size();
    }

    template<typename TServiceInterface>
    void set(const dory::generic::extension::LibraryHandle& libraryHandle, const ServicePointerType<BaseInterfaceType>& service)
    {
        const auto index = getServiceEntryIndex<TServiceInterface>();
        _services[index] = dory::generic::extension::ResourceHandle{ libraryHandle, std::static_pointer_cast<BaseInterfaceType>(service) };
    }

    template<typename TServiceInterface>
    auto get()
    {
        const auto index = getServiceEntryIndex<TServiceInterface>();
        if(auto entry = _services[index])
        {
            return entry->template lock<TServiceInterface>();
        }

        return dory::generic::extension::ResourceRef<ServicePointerType<TServiceInterface>>{ {}, nullptr };
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

    const auto renderService = std::make_shared<RenderService>();
    registry.set<IRenderService>(dory::generic::extension::LibraryHandle{}, renderService);

    auto renderServiceRef = registry.get<IRenderService>();
    EXPECT_TRUE(renderServiceRef.operator bool());
}