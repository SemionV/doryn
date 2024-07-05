#include <dory/registry.h>
#include <dory/sandbox/mainModule.h>
#include <dory/bootstrapper.h>

#ifdef __unix__
int main()
#endif
#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    dory::bootstrapper::StartupModuleContext context;

    auto mainModule = dory::sandbox::MainModule{};
    return mainModule.run(context);
}