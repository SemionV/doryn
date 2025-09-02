#include <dory/profiling/allocationTrack.h>
#include <mutex>
#include "dory/macros/assert.h"

namespace dory::profiling::memory
{
    struct JournalEntry
    {
        void* ptr = nullptr;
        bool alloc = false;
    };

    constexpr std::size_t CAP = 1'000'000;
    static JournalEntry journal[CAP] = {};
    static std::size_t journalCounter = 0;
    static std::mutex mutex;

    bool isActiveAlloc(void* ptr)
    {
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
    }

    void logAlloc(void* ptr)
    {
        std::lock_guard<std::mutex> lock {mutex};

        if(isActiveAlloc(ptr))
        {
            dorySafeDebugBreak();
        }

        journal[journalCounter] = JournalEntry { ptr, true };
        ++journalCounter;
    }

    void logFree(void* ptr)
    {
        std::lock_guard<std::mutex> lock {mutex};

        if(!isActiveAlloc(ptr))
        {
            dorySafeDebugBreak();
        }

        journal[journalCounter] = JournalEntry { ptr, false };
        ++journalCounter;
    }
}