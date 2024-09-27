#include "testModule2.h"

#define API extern "C" BOOST_SYMBOL_EXPORT

API dory::IDynamicModule* dynamicModuleFactory(const std::string& moduleName, client::Registry& registry)
{
    if(moduleName == "testModule2")
    {
        return new testModule::TestModule2(registry);
    }

    return nullptr;
}