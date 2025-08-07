#include <iostream>

void logModuleLoad()
{
    std::cout << "Load Engine Module" << std::endl;
}

void logModuleUnload()
{
    std::cout << "Unload Engine Module" << std::endl;
}

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
    logModuleLoad();
}

__attribute__((destructor))
static void profiler_shutdown()
{
    logModuleUnload();
}

#endif