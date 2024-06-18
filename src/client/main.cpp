#include "registry.h"

#ifdef __unix__
int main()
#endif
#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    auto registry = client::Registry{};

    auto logger = dory::domain::services::StdOutLogService{ "client" };
    auto moduleLoader = dory::domain::services::module::ModuleService<decltype(registry), decltype(logger)>{ logger };

    moduleLoader.load("test module", "modules/testModule", registry);
    moduleLoader.load("test module", "modules/testModule", registry);

    moduleLoader.unload("test module", registry);

    return 0;
}