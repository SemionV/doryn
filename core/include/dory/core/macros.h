#pragma once

#define DORY_DLLEXPORT

#ifdef  DDORY_PLATFORM_WIN32
#ifdef  DORY_DLL_EXPORTS
/*Enabled as "export" while compiling the dll project*/
    #define DORY_DLLEXPORT __declspec(dllexport)
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DORY_DLLEXPORT __declspec(dllimport)
#endif
#endif