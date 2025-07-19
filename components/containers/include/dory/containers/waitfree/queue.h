#pragma once
#include <cstddef>
#include <optional>

namespace dory::containers::waitfree
{
    template<typename T, std::size_t MAX = 16>
    struct CircularBuffer
    {
        constexpr static std::size_t MODULUS = MAX * 2;

        T buffer[MAX] {};
        std::size_t head = 0;
        std::size_t tail = 0;

        //TODO: add a version of add, which is receiving the parameter by value
        bool add(const T& value)
        {
            if(count() == MAX)
            {
                return false;
            }

            buffer[tail % MAX] = value;
            tail = (tail + 1) % MODULUS;

            return true;
        }

        std::optional<T> remove()
        {
            if(count() == 0)
            {
                return {};
            }

            auto value = buffer[head % MAX];
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
