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

        return true;
    }
};

TEST(ConcurrencyTests, CircularBuffer)
{

}