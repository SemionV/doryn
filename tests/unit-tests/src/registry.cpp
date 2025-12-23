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
;
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

template<typename TServiceList>
struct ServicesCount;

template<typename... TServices>
struct ServicesCount<dory::generic::TypeList<TServices...>>
{};

template<typename TServiceList>
class Registry
{
private:
    std::array<dory::generic::extension::ResourceHandle<std::shared_ptr<ServiceInterface>>, 10> _services;
};

TEST(GenericTests, typeList)
{
    std::cout << dory::generic::TypeIndex<IResourceService, ServiceList>::value << std::endl;
    std::cout << dory::generic::ValueIndex<3, IdentifierList>::value << std::endl;
}