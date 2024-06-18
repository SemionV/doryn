#include "client.h"
#include "base/domain/services/logService.h"
#include "base/domain/services/moduleService.h"

int main()
{
    auto registry = client::Registry{};

    auto logger = dory::domain::services::StdOutLogService{ "client" };
    auto moduleLoader = dory::domain::services::module::ModuleService<decltype(registry), decltype(logger)>{logger };

    moduleLoader.load("test module", "modules/testModule", registry);
    moduleLoader.load("test module", "modules/testModule", registry);

    moduleLoader.unload("test module", registry);

    return 0;
}