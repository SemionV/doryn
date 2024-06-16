#include "testModule.h"

#define API extern "C" BOOST_SYMBOL_EXPORT
API std::unique_ptr<dory::ILoadableModule<client::Registry>> loadableModuleFactory()
{
    return std::make_unique<testModule::TestModule>();
}