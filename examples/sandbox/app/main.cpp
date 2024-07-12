#include <dory/registry.h>
#include <dory/sandbox/mainModule.h>
#include <dory/bootstrap.h>

#ifdef __unix__
int main()
#endif
#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    dory::bootstrap::StartupModuleContext context;

    auto mainModule = dory::sandbox::MainModule{};
    return mainModule.run(context);
}