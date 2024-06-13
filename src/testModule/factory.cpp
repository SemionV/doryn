#include "testModule.h"

#define API extern "C" BOOST_SYMBOL_EXPORT
API std::unique_ptr<client::ModuleInterfaceType> moduleFactory()
{
    return std::make_unique<testModule::TestModule>();
}