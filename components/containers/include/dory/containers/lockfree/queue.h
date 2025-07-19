#pragma once
#include <cstddef>
#include <optional>

namespace dory::containers::lockfree
{
    //TODO: align on cacheline
    template <typename T>
    struct Slot
    {
        std::atomic<bool> ready { false };
        T value;
    };

    template<typename T, std::size_t MAX = 16>
    struct CircularBuffer
    {
        using byte = std::uint8_t;
        constexpr static std::size_t MODULUS = MAX * 2;

        Slot<T> buffer[MAX] {}; //TODO: align on sizeof(T)?
        std::size_t head = 0;
        std::atomic<std::size_t> tail = 0;

        //TODO: add a version of add, which is receiving the parameter by value
        bool enqueue(const T& item)
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

        std::optional<T> dequeue()
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
