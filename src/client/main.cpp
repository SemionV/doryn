#include <iostream>
#include "client.h"
#include "base/domain/services/logService.h"
#include "base/domain/services/moduleService.h"

#include <boost/dll/import.hpp>

int main()
{
    auto moduleFactory = boost::dll::import_symbol<dory::PluginFactory<client::Registry>>("modules/testModule.dll", "moduleFactory");

    auto registry = client::Registry{};

    auto module = moduleFactory();
    module->initialize(registry);

    auto logger = dory::domain::services::StdOutLogService{"client"};
    auto moduleLoader = dory::domain::services::module::ModuleLoader<client::Registry, decltype(logger)>{ logger };

    moduleLoader.load("modules/testModule.dll", "test module", registry);

    return 0;
}