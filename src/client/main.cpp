#include <iostream>
#include "client.h"

#include <boost/dll/import.hpp>

int main()
{
    auto moduleFactory = boost::dll::import_symbol<dory::plugin::PluginFactory<client::Registry>>("modules/testModule.dll", "moduleFactory");

    auto registry = client::Registry{};
    auto module = moduleFactory();
    module->initialize(registry);

    return 0;
}