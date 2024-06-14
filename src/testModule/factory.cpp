#include "testModule.h"

#define API extern "C" BOOST_SYMBOL_EXPORT
API std::unique_ptr<dory::IModule<client::Registry>> moduleFactory()
{
    return std::make_unique<testModule::TestModule>();
}