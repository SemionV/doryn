#include "plugin.h"

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