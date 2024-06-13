#include <iostream>
#include "client.h"

#include <boost/dll/import.hpp>

int main()
{
    auto pluginFactory = boost::dll::import_symbol<dory::plugin::PluginFactory<client::Registry>>("testPlugin.dll", "pluginFactory");

    auto registry = client::Registry{};
    auto plugin = pluginFactory();
    plugin->initialize(registry);

    return 0;
}