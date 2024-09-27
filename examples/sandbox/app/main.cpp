#include <dory/sandbox/mainModule.h>

#ifdef DORY_PLATFORM_LINUX
int main()
#endif
#ifdef DORY_PLATFORM_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    dory::bootstrap::StartupModuleContext context;

    auto mainModule = dory::sandbox::MainModule{};
    return mainModule.run(context);
}