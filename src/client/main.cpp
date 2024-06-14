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

    auto moduleState = dory::ModuleStateReferenceType{};

    {
        auto moduleHandle = moduleLoader.load<dory::IModule<client::Registry>>("modules/testModule.dll", "test module");
        moduleState = moduleHandle.state;
        moduleHandle.module->run(moduleState.lock(), registry);

        if(!moduleState.expired())
        {
            logger.information("module is in memory");
        }
    }

    if(!moduleState.expired())
    {
        logger.information("module is in memory");
    }
    else
    {
        logger.information("module is unloaded");
    }

    return 0;
}