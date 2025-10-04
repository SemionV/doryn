#include <cstdlib>
#include <dory/profiling/overrideNewDelete.h>
#include <dory/profiling/allocationTrack.h>

void* operator new(std::size_t sz)
{
    if (void* ptr = std::malloc(sz))
    {
        dory::profiling::memory::logAlloc(ptr, sz);
        return ptr;
    }
    throw std::bad_alloc();
}

void* operator new(std::size_t sz, const std::nothrow_t&) noexcept
{
    void* ptr = std::malloc(sz);

    if(ptr)
    {
        dory::profiling::memory::logAlloc(ptr, sz);
    }

    return ptr;
}

void* operator new[](std::size_t sz)
{
    if (void* ptr = std::malloc(sz))
    {
        dory::profiling::memory::logAlloc(ptr, sz);
        return ptr;
    }
    throw std::bad_alloc();
}

void* operator new[](std::size_t sz, const std::nothrow_t&) noexcept
{
    void* ptr = std::malloc(sz);
    dory::profiling::memory::logAlloc(ptr, sz);
    return ptr;
}

void* operator new(std::size_t sz, std::align_val_t al)
{
    const auto a = static_cast<std::size_t>(al);
#if defined(DORY_PLATFORM_WIN32)
    if (void* ptr = _aligned_malloc(sz, a))
    {
        dory::profiling::memory::logAlloc(ptr, sz);
        return ptr;
    }
#else
    void* ptr = nullptr;
    if (posix_memalign(&ptr, a, sz) == 0)
    {
        dory::profiling::memory::logAlloc(ptr, sz);
        return ptr;
    }
#endif
    throw std::bad_alloc();
}

void* operator new(std::size_t sz, std::align_val_t al, const std::nothrow_t&) noexcept
{
    const auto a = static_cast<std::size_t>(al);
#if defined(DORY_PLATFORM_WIN32)
    void* ptr = _aligned_malloc(sz, a);
#else
    void* ptr = nullptr;
    (void)posix_memalign(&ptr, a, sz);
#endif
    dory::profiling::memory::logAlloc(ptr, sz);
    return ptr;
}

void* operator new[](std::size_t sz, std::align_val_t al)
{
    const auto a = static_cast<std::size_t>(al);
#if defined(DORY_PLATFORM_WIN32)
    if (void* ptr = _aligned_malloc(sz, a))
    {
        dory::profiling::memory::logAlloc(ptr, sz);
        return ptr;
    }
#else
    void* ptr = nullptr;
    if (posix_memalign(&ptr, a, sz) == 0)
    {
        dory::profiling::memory::logAlloc(ptr, sz);
        return ptr;
    }
#endif
    throw std::bad_alloc();
}

void* operator new[](std::size_t sz, std::align_val_t al, const std::nothrow_t&) noexcept
{
    const auto a = static_cast<std::size_t>(al);
#if defined(DORY_PLATFORM_WIN32)
    void* ptr = _aligned_malloc(sz, a);
#else
    void* ptr = nullptr;
    (void)posix_memalign(&ptr, a, sz);
#endif
    dory::profiling::memory::logAlloc(ptr, sz);
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    dory::profiling::memory::logFree(ptr);
    std::free(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept
{
    dory::profiling::memory::logFree(ptr);
    std::free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    dory::profiling::memory::logFree(ptr);
    std::free(ptr);
}

void operator delete[](void* ptr) noexcept
{
    dory::profiling::memory::logFree(ptr);
    std::free(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept
{
    dory::profiling::memory::logFree(ptr);
    std::free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
    dory::profiling::memory::logFree(ptr);
    std::free(ptr);
}

void operator delete(void* ptr, std::align_val_t) noexcept
{
    dory::profiling::memory::logFree(ptr);
#if defined(DORY_PLATFORM_WIN32)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

void operator delete(void* ptr, std::size_t, std::align_val_t) noexcept
{
    dory::profiling::memory::logFree(ptr);
#if defined(DORY_PLATFORM_WIN32)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

void operator delete(void* ptr, std::align_val_t, const std::nothrow_t&) noexcept
{
    dory::profiling::memory::logFree(ptr);
#if defined(DORY_PLATFORM_WIN32)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

void operator delete[](void* ptr, std::align_val_t) noexcept
{
    dory::profiling::memory::logFree(ptr);
#if defined(DORY_PLATFORM_WIN32)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

void operator delete[](void* ptr, std::size_t, std::align_val_t) noexcept
{
    dory::profiling::memory::logFree(ptr);
#if defined(DORY_PLATFORM_WIN32)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

void operator delete[](void* ptr, std::align_val_t, const std::nothrow_t&) noexcept
{
    dory::profiling::memory::logFree(ptr);
#if defined(DORY_PLATFORM_WIN32)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}