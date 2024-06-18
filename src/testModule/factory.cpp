#include "testModule.h"

#define API extern "C" BOOST_SYMBOL_EXPORT
API std::shared_ptr<dory::ILoadableModule<client::Registry>> loadableModuleFactory()
{
    return std::make_shared<testModule::TestModule>();
}