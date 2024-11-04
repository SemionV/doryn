#include <iostream>
#include <boost/config.hpp>
#include <extension.h>
#include <dory/core/resources/dataContext.h>

/*#ifdef __unix__
__attribute__((constructor))
void on_load() {
    std::cout << "Extension Library loaded" << std::endl;
}

__attribute__((destructor))
void on_unload() {
    std::cout << "Extension Library unloaded" << std::endl;
}
#endif*/

#define API extern "C" BOOST_SYMBOL_EXPORT

API dory::generic::extension::IModule<dory::core::resources::DataContext>* moduleFactory(const std::string& moduleName, dory::core::Registry& registry)
{
    if(moduleName == "test-extension")
    {
        return new dory::game::test_extension::Extension(registry);
    }

    return nullptr;
}