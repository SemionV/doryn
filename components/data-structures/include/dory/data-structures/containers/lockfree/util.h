#pragma once

#include <thread>

namespace dory::data_structures::containers::lockfree
{
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#include <immintrin.h>
    static inline void cpu_relax() noexcept { _mm_pause(); }
#else
    static inline void cpu_relax() noexcept { std::this_thread::yield(); }
#endif
}