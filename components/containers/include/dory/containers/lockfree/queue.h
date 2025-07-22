#pragma once
#include <cstddef>
#include <optional>
#include <dory/macros/constants.h>

namespace dory::containers::lockfree
{
    template <typename T, std::size_t ALIGN = constants::CACHELINE_SIZE>
    struct alignas(ALIGN) Slot
    {
        T value;
        std::atomic<bool> ready { false };
    };

    //Multiple Producer - Multiple Consumer bounded queue
    template<typename T, typename TSlot = Slot<T>, std::size_t MAX = 16>
    requires((MAX & (MAX - 1)) == 0) // MAX is a power of 2
    struct CircularBuffer
    {
        constexpr static std::size_t MODULUS = MAX * 2;
        constexpr static std::size_t MODULUS_MASK = MODULUS - 1;
        constexpr static std::size_t MAX_MOD_MASK = MAX - 1;

        TSlot buffer[MAX] {};
        std::atomic<std::size_t> head = 0;
        std::atomic<std::size_t> tail = 0;

        static std::size_t mod(const std::size_t index)
        {
            return index & MODULUS_MASK;
        }

        static std::size_t bufferMod(const std::size_t index)
        {
            return index & MAX_MOD_MASK;
        }

        bool push(T value)
        {
            while(true)
            {
                std::size_t currentTail = tail.load(std::memory_order::relaxed);
                if(mod(currentTail - head.load(std::memory_order::relaxed) + MODULUS) == MAX)
                {
                    return false; //buffer is full
                }

                const std::size_t newTail = mod(currentTail + 1);
                if(!tail.compare_exchange_weak(currentTail, newTail, std::memory_order::release, std::memory_order::relaxed))
                {
                    continue; //the slot is occupied by another thread, try next slot(allows spurious failure as well)
                }

                TSlot& slot = buffer[bufferMod(currentTail)];

                slot.value = value;
                slot.ready.store(true, std::memory_order::release);

                return true;
            }
        }

        std::optional<T> pop()
        {
            while(true)
            {
                std::size_t currentHead = head.load(std::memory_order::relaxed);
                if(mod(tail.load(std::memory_order::relaxed) - currentHead + MODULUS) == 0)
                {
                    return {}; //buffer is empty
                }

                TSlot& slot = buffer[bufferMod(currentHead)];
                if(!slot.ready.load(std::memory_order::acquire))
                {
                    return {}; // some thread is still writing to the slot, consider the queue is empty
                }

                const std::size_t newHead = mod(currentHead + 1);
                if(!head.compare_exchange_weak(currentHead, newHead, std::memory_order::release, std::memory_order::relaxed))
                {
                    continue; // slot not yet marked ready, treat as empty for now(allows spurious failure as well)
                }

                auto value = slot.value;
                slot.ready.store(false, std::memory_order::release); //make slot free for a data write

                return value;
            }
        }

        void clear()
        {
            head.store(0, std::memory_order::relaxed);
            tail.store(0, std::memory_order::relaxed);
            for (std::size_t i = 0; i < MAX; ++i)
            {
                buffer[i].ready.store(false, std::memory_order::relaxed);
            }
        }

        [[nodiscard]] std::size_t count() const
        {
            return mod(tail - head + MODULUS);
        }

        const TSlot* data() const
        {
            return buffer;
        }
    };
}
