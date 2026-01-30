#pragma once

#include <atomic>
#include "util.h"

namespace dory::data_structures::containers::lockfree
{
    /*
     * A reader-writer lock with bias towards readers(prefers readers to acquire lock fast)
     */
    class SharedLock
    {
    private:
        std::atomic<int32_t> _state {0};

    public:
        void lock_shared() noexcept
        {
            while(true)
            {
                int32_t state = _state.load(std::memory_order::acquire);

                if(state < 0)
                {
                    cpu_relax();
                    continue;
                }

                if(_state.compare_exchange_weak(state, state + 1, std::memory_order_acquire, std::memory_order_relaxed))
                {
                    return;
                }
            }
        }

        void unlock_shared() noexcept
        {
            _state.fetch_sub(1, std::memory_order_release);
        }

        void lock() noexcept
        {
            while(true)
            {
                int32_t expected = 0;

                if(_state.compare_exchange_weak(expected, -1, std::memory_order_acquire, std::memory_order_relaxed))
                {
                    return;
                }

                cpu_relax();
            }
        }

        void unlock() noexcept
        {
            _state.store(0, std::memory_order_release);
        }
    };
}