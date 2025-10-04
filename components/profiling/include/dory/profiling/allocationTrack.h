#pragma once

#include <cstddef>

#ifdef  DORY_PLATFORM_WIN32
    #ifdef  DORY_ALLOC_BUILD
    /*Enabled as "export" while compiling the dll project*/
    #define DORY_ALLOC_API __declspec(dllexport)
    #else
    /*Enabled as "import" in the Client side for using already created dll file*/
    #define DORY_ALLOC_API __declspec(dllimport)
    #endif
#else
    #if defined(DORY_ALLOC_BUILD)
        #define DORY_ALLOC_API __attribute__((visibility("default")))
    #else
        #define DORY_ALLOC_API
    #endif
#endif

namespace dory::profiling::memory
{
    DORY_ALLOC_API bool isActiveAlloc(void* ptr);
    DORY_ALLOC_API void logAlloc(void* ptr, std::size_t size);
    DORY_ALLOC_API void logFree(void* ptr);
}