#pragma once
#include <new>        // std::nothrow_t, std::align_val_t
#include <cstddef>
#include <cstdlib>
#include <mutex>

#if defined(_WIN32)
#include <malloc.h> // _aligned_malloc/_aligned_free
#else
#include <stdlib.h> // posix_memalign
#endif

#include <dory/profiling/profiler.h>

// ---------- Config ----------
#ifndef DORY_TRACY_TRAP_DOUBLE_ALLOC
#define DORY_TRACY_TRAP_DOUBLE_ALLOC 1   // set 1 to break on true duplicate allocs we TRACKED
#endif
#ifndef DORY_TRACY_TRAP_UNKNOWN_FREE
#define DORY_TRACY_TRAP_UNKNOWN_FREE 0   // set 1 to break on frees we never TRACKED
#endif

#if defined(_WIN32)
#define DORY_TRAP() __debugbreak()
#else
#define DORY_TRAP() __builtin_trap()
#endif

// ---------- Fixed-capacity pointer set (no heap) ----------
namespace dory_dbgset {
    constexpr std::size_t CAP = 1'000'000;
    static const void* slots[CAP] = {};
    static std::mutex m;

    inline bool insert(const void* p) {
        std::lock_guard<std::mutex> lk(m);
        for (std::size_t i = 0; i < CAP; ++i)
            if (slots[i] == p) return false;         // already tracked
        for (std::size_t i = 0; i < CAP; ++i)
            if (slots[i] == nullptr) { slots[i] = p; return true; }
        return false; // table full -> treat as already present
    }
    inline bool erase_if_present(const void* p) {
        std::lock_guard<std::mutex> lk(m);
        for (std::size_t i = 0; i < CAP; ++i)
            if (slots[i] == p) { slots[i] = nullptr; return true; }
        return false;
    }
    inline void clear_all() {
        std::lock_guard<std::mutex> lk(m);
        for (std::size_t i = 0; i < CAP; ++i) slots[i] = nullptr;
    }
}

// ---------- Trace helpers ----------
static constexpr const char* kDoryPool = "System";

// Track ALWAYS (so alloc/free pairs survive the "ready" flip) …
static inline void track_alloc(void* p) noexcept {
    if (!p) return;
#if DORY_TRACY_TRAP_DOUBLE_ALLOC
    if (!dory_dbgset::insert(p)) DORY_TRAP();
#else
    (void)dory_dbgset::insert(p);
#endif
}
static inline bool track_free(void* p) noexcept {
    if (!p) return false;
    bool had = dory_dbgset::erase_if_present(p);
#if DORY_TRACY_TRAP_UNKNOWN_FREE
    if (!had) DORY_TRAP();
#endif
    return had;
}

// …but emit Tracy events ONLY when ready
static inline void tracy_alloc_if_ready(void* p, std::size_t sz) noexcept {
    if (p && DORY_TRACE_IS_PROFILER_READY) DORY_TRACE_MEM_ALLOC(p, sz, kDoryPool);
}
static inline void tracy_free_if_ready(void* p) noexcept {
    if (p && DORY_TRACE_IS_PROFILER_READY) DORY_TRACE_MEM_FREE(p, kDoryPool);
}

// Call this right before you flip your ready flag, if you want a clean slate:
//   dory_dbgset::clear_all(); DORY_TRACE_SET_PROFILER_READY();

// =======================================
// Global new/delete overrides (C++17+)
// Covers: scalar/array, sized, aligned, nothrow
// =======================================

// ---- scalar new/delete (unaligned) ----
void* operator new(std::size_t sz) {
    if (void* p = std::malloc(sz)) { track_alloc(p); tracy_alloc_if_ready(p, sz); return p; }
    throw std::bad_alloc();
}
void* operator new(std::size_t sz, const std::nothrow_t&) noexcept {
    void* p = std::malloc(sz);
    track_alloc(p); tracy_alloc_if_ready(p, sz);
    return p;
}
void  operator delete(void* p) noexcept {
    if (track_free(p)) tracy_free_if_ready(p);
    std::free(p);
}
void  operator delete(void* p, std::size_t) noexcept {
    if (track_free(p)) tracy_free_if_ready(p);
    std::free(p);
}
void  operator delete(void* p, const std::nothrow_t&) noexcept {
    if (track_free(p)) tracy_free_if_ready(p);
    std::free(p);
}

// ---- array new/delete (unaligned) ----
void* operator new[](std::size_t sz) {
    if (void* p = std::malloc(sz)) { track_alloc(p); tracy_alloc_if_ready(p, sz); return p; }
    throw std::bad_alloc();
}
void* operator new[](std::size_t sz, const std::nothrow_t&) noexcept {
    void* p = std::malloc(sz);
    track_alloc(p); tracy_alloc_if_ready(p, sz);
    return p;
}
void  operator delete[](void* p) noexcept {
    if (track_free(p)) tracy_free_if_ready(p);
    std::free(p);
}
void  operator delete[](void* p, std::size_t) noexcept {
    if (track_free(p)) tracy_free_if_ready(p);
    std::free(p);
}
void  operator delete[](void* p, const std::nothrow_t&) noexcept {
    if (track_free(p)) tracy_free_if_ready(p);
    std::free(p);
}

// ---- aligned scalar new/delete ----
void* operator new(std::size_t sz, std::align_val_t al) {
    const std::size_t a = static_cast<std::size_t>(al);
#if defined(_WIN32)
    if (void* p = _aligned_malloc(sz, a)) { track_alloc(p); tracy_alloc_if_ready(p, sz); return p; }
#else
    void* p = nullptr;
    if (posix_memalign(&p, a, sz) == 0) { track_alloc(p); tracy_alloc_if_ready(p, sz); return p; }
#endif
    throw std::bad_alloc();
}
void* operator new(std::size_t sz, std::align_val_t al, const std::nothrow_t&) noexcept {
    const std::size_t a = static_cast<std::size_t>(al);
#if defined(_WIN32)
    void* p = _aligned_malloc(sz, a);
#else
    void* p = nullptr;
    (void)posix_memalign(&p, a, sz);
#endif
    track_alloc(p); tracy_alloc_if_ready(p, sz);
    return p;
}
void  operator delete(void* p, std::align_val_t) noexcept {
    if (track_free(p)) tracy_free_if_ready(p);
#if defined(_WIN32)
    _aligned_free(p);
#else
    std::free(p);
#endif
}
void  operator delete(void* p, std::size_t, std::align_val_t) noexcept {
    if (track_free(p)) tracy_free_if_ready(p);
#if defined(_WIN32)
    _aligned_free(p);
#else
    std::free(p);
#endif
}
void  operator delete(void* p, std::align_val_t, const std::nothrow_t&) noexcept {
    if (track_free(p)) tracy_free_if_ready(p);
#if defined(_WIN32)
    _aligned_free(p);
#else
    std::free(p);
#endif
}

// ---- aligned array new/delete ----
void* operator new[](std::size_t sz, std::align_val_t al) {
    const std::size_t a = static_cast<std::size_t>(al);
#if defined(_WIN32)
    if (void* p = _aligned_malloc(sz, a)) { track_alloc(p); tracy_alloc_if_ready(p, sz); return p; }
#else
    void* p = nullptr;
    if (posix_memalign(&p, a, sz) == 0) { track_alloc(p); tracy_alloc_if_ready(p, sz); return p; }
#endif
    throw std::bad_alloc();
}
void* operator new[](std::size_t sz, std::align_val_t al, const std::nothrow_t&) noexcept {
    const std::size_t a = static_cast<std::size_t>(al);
#if defined(_WIN32)
    void* p = _aligned_malloc(sz, a);
#else
    void* p = nullptr;
    (void)posix_memalign(&p, a, sz);
#endif
    track_alloc(p); tracy_alloc_if_ready(p, sz);
    return p;
}
void  operator delete[](void* p, std::align_val_t) noexcept {
    if (track_free(p)) tracy_free_if_ready(p);
#if defined(_WIN32)
    _aligned_free(p);
#else
    std::free(p);
#endif
}
void  operator delete[](void* p, std::size_t, std::align_val_t) noexcept {
    if (track_free(p)) tracy_free_if_ready(p);
#if defined(_WIN32)
    _aligned_free(p);
#else
    std::free(p);
#endif
}
void  operator delete[](void* p, std::align_val_t, const std::nothrow_t&) noexcept {
    if (track_free(p)) tracy_free_if_ready(p);
#if defined(_WIN32)
    _aligned_free(p);
#else
    std::free(p);
#endif
}
