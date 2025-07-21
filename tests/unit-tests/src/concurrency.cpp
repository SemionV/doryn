#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/containers/lockfree/queue.h>
#include <spdlog/fmt/fmt.h>

using namespace dory::containers::lockfree;

TEST(ConcurrencyTests, CircularBufferBoundaries)
{
    constexpr std::size_t MAX = 4;

    CircularBuffer<int, Slot<int>, MAX> queue;

    for(std::size_t i = 0; i < MAX + 1; ++i)
    {
        int value = (int)i + 1;
        std::cout << fmt::format("Push value: {}", value) << std::endl;
        queue.push(value);
    }

    for(std::size_t i = 0; i < MAX + 1; ++i)
    {
        auto value = queue.pop();
        std::cout << fmt::format("Pop value: {}", value ? *value : -1) << std::endl;
    }
}