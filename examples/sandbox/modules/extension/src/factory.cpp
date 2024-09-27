#include <boost/config.hpp>
#include <extension.h>

#define API extern "C" BOOST_SYMBOL_EXPORT

API dory::IDynamicModule* dynamicModuleFactory(const std::string& moduleName, dory::sandbox::ExtensionContext& extensionContext)
{
    if(moduleName == "extension")
    {
        return new dory::sandbox::Extension(extensionContext);
    }

    return nullptr;
}