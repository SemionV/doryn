#include <dory/profiling/allocationTrack.h>
#include <mutex>
#include "dory/macros/assert.h"
#include <iostream>
#include <dory/profiling/profiler.h>

static auto systemMemPoolName = "System";

namespace dory::profiling::memory
{
#ifdef DORY_PROFILING_MEMORY_JOURNAL
    struct JournalEntry
    {
        void* ptr = nullptr;
        bool alloc = false;
    };

    constexpr std::size_t CAP = 1'000'000;
    static JournalEntry journal[CAP] = {};
    static std::size_t journalCounter = 0;
    static std::mutex mutex;
#endif

    bool isActiveAlloc(void* ptr)
    {
#ifdef DORY_PROFILING_MEMORY_JOURNAL
        bool isActive = false;

        for(std::size_t i = 0; i <= journalCounter; ++i)
        {
            auto entry = journal[i];
            if(entry.ptr == ptr)
            {
                isActive = entry.alloc;
            }
        }

        return isActive;
#else
        return false;
#endif
    }

    void logAlloc(void* ptr, std::size_t size)
    {
#ifdef DORY_PROFILING_MEMORY_JOURNAL
        {
            std::lock_guard lock { mutex };

            if(isActiveAlloc(ptr))
            {
                dorySafeDebugBreak();
            }

            journal[journalCounter] = JournalEntry { ptr, true };
            ++journalCounter;
        }
#endif

        if(DORY_TRACE_IS_PROFILER_READY)
        {
            DORY_TRACE_MEM_ALLOC(ptr, size, systemMemPoolName);
        }
    }

    void logFree(void* ptr)
    {
#ifdef DORY_PROFILING_MEMORY_JOURNAL
        {
            std::lock_guard lock { mutex };

            if(!isActiveAlloc(ptr))
            {
                dorySafeDebugBreak();
            }

            journal[journalCounter] = JournalEntry { ptr, false };
            ++journalCounter;
        }
#endif

        if(DORY_TRACE_IS_PROFILER_READY)
        {
            DORY_TRACE_MEM_FREE(ptr, systemMemPoolName);
        }
    }
}