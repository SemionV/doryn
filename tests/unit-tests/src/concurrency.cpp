#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/containers/waitfree/queue.h>
#include <spdlog/fmt/fmt.h>

using namespace dory::containers::waitfree;

template<typename T, std::size_t MAX>
void printStatus(CircularBuffer<T, MAX> queue)
{
    std::size_t count = queue.count();
    std::cout << fmt::format("Count: {}, Items: ", count);
    const T* data = queue.data();
    for(std::size_t i = 0; i < count; ++i)
    {
        if(i > 0)
        {
            std::cout << ",";
        }

        std::cout << data[i];
    }

    std::cout << ";" << std::endl;
}

TEST(ConcurrencyTests, BitwiseModulo)
{
    constexpr std::size_t value = 12;
    std::cout << (value >> 3) << std::endl;
}

TEST(ConcurrencyTests, CircularBufferBoundaries)
{
    constexpr std::size_t MAX = 4;

    CircularBuffer<int, MAX> queue;

    for(std::size_t i = 0; i < MAX + 1; ++i)
    {
        queue.add(i + 1);
        printStatus(queue);
    }

    for(std::size_t i = 0; i < MAX + 1; ++i)
    {
        queue.remove();
        printStatus(queue);
    }
}