#include <dory/sandbox/mainModule.h>

#ifdef DORY_MAIN_FUNCTION_UNIX
int main()
#endif
#ifdef DORY_MAIN_FUNCTION_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    dory::bootstrap::StartupModuleContext context;

    auto mainModule = dory::sandbox::MainModule{};
    return mainModule.run(context);
}