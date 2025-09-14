#pragma once

#include <cstddef>

#if defined(DORY_ALLOC_BUILD)
    #define DORY_ALLOC_API __declspec(dllexport)
#else
    #define DORY_ALLOC_API __declspec(dllimport)
#endif

namespace dory::profiling::memory
{
    DORY_ALLOC_API bool isActiveAlloc(void* ptr);
    DORY_ALLOC_API void logAlloc(void* ptr, std::size_t size);
    DORY_ALLOC_API void logFree(void* ptr);
}