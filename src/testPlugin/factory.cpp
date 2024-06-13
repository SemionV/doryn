#include "plugin.h"

#define API extern "C" BOOST_SYMBOL_EXPORT
API std::unique_ptr<client::PluginInterfaceType> pluginFactory()
{
    return std::make_unique<testPlugin::TestPlugin>();
}