#pragma once
#include <cstddef>
#include <optional>

namespace dory::containers::lockfree
{
    //align to cacheline to avoid false sharing!
    template <typename T>
    struct Slot
    {
        std::atomic<bool> ready { false };
        T value;
    };

    //Multiple Producer - Multiple Consumer queue
    template<typename T, typename TSlot = Slot<T>, std::size_t MAX = 16>
    struct CircularBuffer
    {
        constexpr static std::size_t MODULUS = MAX * 2;

        TSlot buffer[MAX] {};
        std::atomic<std::size_t> head = 0;
        std::atomic<std::size_t> tail = 0;

        bool push(T value)
        {
            while(true)
            {
                std::size_t currentTail = tail.load(std::memory_order::acquire);
                if((currentTail - head + MODULUS) % MODULUS == MAX)
                {
                    return false; //buffer is full
                }

                const std::size_t newTail = (currentTail + 1) % MODULUS;
                if(!tail.compare_exchange_strong(currentTail, newTail, std::memory_order::release, std::memory_order::release))
                {
                    continue; //the slot is occupied by another thread, try next slot
                }

                TSlot& slot = buffer[currentTail];

                if (!slot.ready.compare_exchange_strong(true, false, std::memory_order::acquire))
                {
                    continue; //Another thread is writing to the slot, this cannot happen, but the check is useful anyway. Try again.
                }

                slot.value = value;
                slot.ready.store(true, std::memory_order::release);

                return true;
            }
        }

        std::optional<T> pop()
        {
            while(true)
            {
                std::size_t currentHead = tail.load(std::memory_order::acquire);
                if((tail - currentHead + MODULUS) % MODULUS == 0)
                {
                    return false; //buffer is empty
                }

                //TODO: have to use ready flag as spin-lock, currently it is unsafe to return  value
                TSlot& slot = buffer[currentHead];
                if(!slot.ready.load(std::memory_order::relaxed))
                {
                    return false; //some thread is still writing to the slot, consider the queue is empty
                }

                const std::size_t newHead = (currentHead + 1) % MODULUS;
                if(!head.compare_exchange_strong(currentHead, newHead, std::memory_order::release, std::memory_order::release))
                {
                    continue; //the slot is occupied by another thread, try next slot
                }

                //TODO: what if the ready flag has changed by another thread and the value is invalid?

                return slot.value;
            }
        }

        [[nodiscard]] std::size_t count() const
        {
            return (tail - head + MODULUS) % MODULUS;
        }

        const T* data()
        {
            return buffer;
        }
    };

    template<typename T, std::size_t MAX = 16>
    struct CircularBufferMPSC
    {
        using byte = std::uint8_t;
        constexpr static std::size_t MODULUS = MAX * 2;

        Slot<T> buffer[MAX] {}; //TODO: align on sizeof(T)?
        std::size_t head = 0;
        std::atomic<std::size_t> tail = 0;

        //TODO: add a version of add, which is receiving the parameter by value
        bool push(const T& item)
        {
            size_t pos = tail.fetch_add(1, std::memory_order_relaxed) % MAX;

            Slot<T>& slot = buffer[pos];

            // Spin until the slot is available (not yet consumed)
            bool expected = false;
            if (!slot.ready.compare_exchange_strong(expected, true, std::memory_order_acquire))
            {
                return false; // Slot still occupied – treat as full
            }

            slot.value = item;
            // Publish the value
            slot.ready.store(true, std::memory_order_release);
            return true;
        }

        std::optional<T> pop()
        {
            if(count() == 0)
            {
                return {};
            }

            Slot<T>& slot = buffer[head % MAX];
            if (!slot.ready.load(std::memory_order_acquire))
            {
                return {}; // not yet written
            }

            T value = slot.value;
            slot.ready.store(false, std::memory_order_release); // mark as free

            head = (head + 1) % MODULUS;

            return value;
        }

        [[nodiscard]] std::size_t count() const
        {
            return (tail - head + MODULUS) % MODULUS;
        }

        const T* data()
        {
            return buffer;
        }
    };
}
