#include <dory/sandbox/mainModule.h>

#ifdef __unix__
int main()
#endif
#if (defined(_WIN32) || defined(_WIN64) || defined(WIN32))
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    dory::bootstrap::StartupModuleContext context;

    auto mainModule = dory::sandbox::MainModule{};
    return mainModule.run(context);
}