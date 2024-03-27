#include "dependencies.h"
#include "registry.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
{
    using namespace testApp;

    auto services = registry::ServicesLocal{};
    auto dataContext = registry::DataContextType{};

    services.project.run(dataContext);

    return 0;
}