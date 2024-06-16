#include <iostream>
#include "client.h"
#include "base/domain/services/logService.h"
#include "base/domain/services/moduleService.h"

#include <boost/dll/import.hpp>

int main()
{
    auto registry = client::Registry{};

    auto logger = dory::domain::services::StdOutLogService{"client"};
    auto moduleLoader = dory::domain::services::module::ModuleLoader<decltype(logger)>{ logger };

    auto moduleHandle = dory::ModuleHandle{ "test module", "modules/testModule"};
    auto module = moduleLoader.load<client::Registry>(moduleHandle);
    if(module)
    {
        module->load(moduleHandle, registry);
    }

    return 0;
}