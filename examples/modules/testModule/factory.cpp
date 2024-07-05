#include "testModule.h"
#include "testModule2.h"

#define API extern "C" BOOST_SYMBOL_EXPORT
API std::shared_ptr<dory::ILoadableModule<client::Registry>> loadableModuleFactory()
{
    return std::make_shared<testModule::TestModule>();
}

API std::shared_ptr<dory::IDynamicModule<client::Registry>> dynamicModuleFactory(const std::string& moduleName)
{
    if(moduleName == "testModule2")
    {
        return std::make_shared<testModule::TestModule2>();
    }

    return nullptr;
}