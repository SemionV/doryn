#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/generic/typeList.h>

class IRenderService;
class IResourceService;
class IAudioService;

using ServiceList = dory::generic::TypeList<
        IRenderService,
        IResourceService,
        IAudioService
    >;
;
using IdentifierList = dory::generic::ValueList<int, 1, 2, 3, 4, 5>;

template<typename ServiceList>
struct Registry
{

};

TEST(GenericTests, typeList)
{
    std::cout << dory::generic::TypeIndex<IResourceService, ServiceList>::value << std::endl;
    std::cout << dory::generic::ValueIndex<3, IdentifierList>::value << std::endl;
}