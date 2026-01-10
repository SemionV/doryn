#pragma once
#include <atomic>

namespace dory::data_structures::containers::lockfree
{
    class SpinLockMutex
    {
        std::atomic_flag _flag{};

      public:
        void lock() noexcept
        {
            while (_flag.test_and_set(std::memory_order_acquire))
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
                // Since C++20, locks can be acquired only after notification in the unlock,
                // avoiding any unnecessary spinning.
                // Note that even though wait guarantees it returns only after the value has
                // changed, the lock is acquired after the next condition check.
                _flag.wait(true, std::memory_order_relaxed)
#endif
                    ;
        }

        bool try_lock() noexcept
        {
            return !_flag.test_and_set(std::memory_order_acquire);
        }

        void unlock() noexcept
        {
            _flag.clear(std::memory_order_release);
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
            _flag.notify_one();
#endif
        }
    };
}
