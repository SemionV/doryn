#include <gtest/gtest.h>
#include <gmock/gmock.h>

template<typename T>
struct CircularBuffer
{
    constexpr static std::size_t MAX = 16;
    constexpr static std::size_t MODULUS = MAX * 2;

    T buffer[MAX] {};
    std::size_t head = 0;
    std::size_t tail = 0;

    bool add(const T& value)
    {
        if((tail - head + MODULUS) % MODULUS == MAX)
        {
            return false;
        }

        buffer[tail % MAX] = value;
        tail = (tail + 1) % MODULUS;

        return true;
    }

    std::optional<T> remove()
    {
        if((tail - head + MODULUS) % MODULUS == 0)
        {
            return {};
        }

        auto value = buffer[head % MAX];
        head = (head + 1) % MODULUS;

        return value;
    }
};

TEST(ConcurrencyTests, CircularBuffer)
{
    CircularBuffer<int> buffer;

    buffer.add(1);
    buffer.add(2);
    buffer.add(3);
    buffer.add(4);
    buffer.add(5);
    buffer.add(6);
    buffer.add(7);
    buffer.add(8);
    buffer.add(9);
    buffer.add(10);
    buffer.add(11);
    buffer.add(12);
    buffer.add(13);
    buffer.add(14);
    buffer.add(15);
    buffer.add(16);
    buffer.add(17);

    buffer.remove();
    buffer.remove();
    buffer.remove();
    buffer.remove();
}