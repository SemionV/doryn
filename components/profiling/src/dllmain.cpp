
#include <iostream>

#if defined(_WIN32)
#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE, DWORD reason, LPVOID)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        logModuleLoad();
        break;
    case DLL_PROCESS_DETACH:
        logModuleUnload();
        break;
    }
    return TRUE;
}

#elif defined(__linux__) || defined(__APPLE__)

// GCC/Clang: constructor runs on .so load, destructor runs on unload
__attribute__((constructor))
static void profiler_init()
{
    std::cout << "Load Profiler Module" << std::endl;
}

__attribute__((destructor))
static void profiler_shutdown()
{
    std::cout << "Unload Profiler Module" << std::endl;
}

#endif