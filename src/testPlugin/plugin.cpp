#include "plugin.h"

namespace testPlugin
{
    void TestPlugin::initialize(client::Registry &registry)
    {

    }
}

namespace testPlugin
{
    static std::unique_ptr<client::PluginInterfaceType> pluginFactory()
    {
        return std::make_unique<TestPlugin>();
    }
}

BOOST_DLL_ALIAS
(
        testPlugin::pluginFactory,
        pluginFactory
)

#define API extern "C" BOOST_SYMBOL_EXPORT
API int i_am_a_cpp11_function(std::string&& param) noexcept;

int i_am_a_cpp11_function(std::string&& param) noexcept
{
    return 12;
}