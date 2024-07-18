#include <boost/config.hpp>
#include <extension.h>

#define API extern "C" BOOST_SYMBOL_EXPORT

API std::shared_ptr<dory::IDynamicModule<dory::sandbox::ExtensionContext>> dynamicModuleFactory(const std::string& moduleName,
                                                                                                dory::sandbox::ExtensionContext& extensionContext)
{
    if(moduleName == "extension")
    {
        return std::make_shared<dory::sandbox::Extension>(extensionContext);
    }

    return nullptr;
}