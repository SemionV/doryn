#include "testModule2.h"

#define API extern "C" BOOST_SYMBOL_EXPORT

API std::shared_ptr<dory::IDynamicModule<client::Registry>> dynamicModuleFactory(const std::string& moduleName, client::Registry& registry)
{
    if(moduleName == "testModule2")
    {
        return std::make_shared<testModule::TestModule2>(registry);
    }

    return nullptr;
}