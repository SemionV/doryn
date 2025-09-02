#pragma once

#include <dory/macros/assert.h>

namespace dory::memory_trace
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

void* operator new(std::size_t sz)
{
    if (void* ptr = std::malloc(sz))
    {
        dory::memory_trace::logAlloc(ptr);
        return ptr;
    }
    throw std::bad_alloc();
}

void* operator new(std::size_t sz, const std::nothrow_t&) noexcept
{
    void* ptr = std::malloc(sz);

    if(ptr)
    {
        dory::memory_trace::logAlloc(ptr);
    }

    return ptr;
}

void  operator delete(void* ptr) noexcept
{
    dory::memory_trace::logFree(ptr);
    std::free(ptr);
}

void  operator delete(void* ptr, std::size_t) noexcept
{
    dory::memory_trace::logFree(ptr);
    std::free(ptr);
}

void  operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    dory::memory_trace::logFree(ptr);
    std::free(ptr);
}

// ---- array new/delete (unaligned) ----
void* operator new[](std::size_t sz)
{
    if (void* ptr = std::malloc(sz))
    {
        dory::memory_trace::logAlloc(ptr);
        return ptr;
    }
    throw std::bad_alloc();
}

void* operator new[](std::size_t sz, const std::nothrow_t&) noexcept
{
    void* ptr = std::malloc(sz);
    dory::memory_trace::logAlloc(ptr);
    return ptr;
}

void  operator delete[](void* ptr) noexcept
{
    dory::memory_trace::logFree(ptr);
    std::free(ptr);
}

void  operator delete[](void* ptr, std::size_t) noexcept
{
    dory::memory_trace::logFree(ptr);
    std::free(ptr);
}

void  operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
    dory::memory_trace::logFree(ptr);
    std::free(ptr);
}

// ---- aligned scalar new/delete ----
void* operator new(std::size_t sz, std::align_val_t al)
{
    const auto a = static_cast<std::size_t>(al);
#if defined(_WIN32)
    if (void* ptr = _aligned_malloc(sz, a))
    {
        dory::memory_trace::logAlloc(ptr);
        return ptr;
    }
#else
    void* p = nullptr;
    if (posix_memalign(&ptr, a, sz) == 0)
    {
        dory::memory_trace::logAlloc(ptr);
        return ptr;
    }
#endif
    throw std::bad_alloc();
}

void* operator new(std::size_t sz, std::align_val_t al, const std::nothrow_t&) noexcept
{
    const auto a = static_cast<std::size_t>(al);
#if defined(_WIN32)
    void* ptr = _aligned_malloc(sz, a);
#else
    void* ptr = nullptr;
    (void)posix_memalign(&ptr, a, sz);
#endif
    dory::memory_trace::logAlloc(ptr);
    return ptr;
}

void  operator delete(void* ptr, std::align_val_t) noexcept
{
    dory::memory_trace::logFree(ptr);
#if defined(_WIN32)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

void  operator delete(void* ptr, std::size_t, std::align_val_t) noexcept
{
    dory::memory_trace::logFree(ptr);
#if defined(_WIN32)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

void  operator delete(void* ptr, std::align_val_t, const std::nothrow_t&) noexcept
{
    dory::memory_trace::logFree(ptr);
#if defined(_WIN32)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

// ---- aligned array new/delete ----
void* operator new[](std::size_t sz, std::align_val_t al)
{
    const auto a = static_cast<std::size_t>(al);
#if defined(_WIN32)
    if (void* ptr = _aligned_malloc(sz, a))
    {
        dory::memory_trace::logAlloc(ptr);
        return ptr;
    }
#else
    void* ptr = nullptr;
    if (posix_memalign(&ptr, a, sz) == 0)
    {
        dory::memory_trace::logAlloc(ptr);
        return ptr;
    }
#endif
    throw std::bad_alloc();
}

void* operator new[](std::size_t sz, std::align_val_t al, const std::nothrow_t&) noexcept
{
    const auto a = static_cast<std::size_t>(al);
#if defined(_WIN32)
    void* ptr = _aligned_malloc(sz, a);
#else
    void* ptr = nullptr;
    (void)posix_memalign(&ptr, a, sz);
#endif
    dory::memory_trace::logAlloc(ptr);
    return ptr;
}

void  operator delete[](void* ptr, std::align_val_t) noexcept
{
    dory::memory_trace::logFree(ptr);
#if defined(_WIN32)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

void  operator delete[](void* ptr, std::size_t, std::align_val_t) noexcept
{
    dory::memory_trace::logFree(ptr);
#if defined(_WIN32)
    _aligned_free(ptr);
#else
    std::free(p);
#endif
}

void  operator delete[](void* ptr, std::align_val_t, const std::nothrow_t&) noexcept
{
    dory::memory_trace::logFree(ptr);
#if defined(_WIN32)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}