#pragma once

#ifdef  DORY_PLATFORM_WIN32
    #ifdef  DORY_DLL_EXPORTS
    /*Enabled as "export" while compiling the dll project*/
    #define DORY_DLL_API __declspec(dllexport)
    #else
    /*Enabled as "import" in the Client side for using already created dll file*/
    #define DORY_DLL_API __declspec(dllimport)
    #endif
#else
    #if defined(DORY_DLL_EXPORTS)
        #define DORY_DLL_API __attribute__((visibility("default")))
    #else
        #define DORY_DLL_API
    #endif
#endif
