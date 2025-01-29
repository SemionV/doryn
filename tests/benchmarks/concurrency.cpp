#include <benchmark/benchmark.h>

#include <dory/concurrency/queue.h>

#include <dory/concurrency/log.h>
#include <dory/concurrency/messaging.h>

static const constexpr std::size_t largeDataCount = 10000;
static const constexpr std::size_t smallDataCount = 10;

struct QuitMessage
{
};

struct TestMessage
{
    std::size_t& counter;
    std::atomic<bool>& processed;

    explicit TestMessage(std::size_t& counter, std::atomic<bool>& processed):
        counter(counter),
        processed(processed)
    {}
};

static void BM_sendMessage(benchmark::State& state) {
    dory::concurrency::messaging::SingleMessageRecieverHub<TestMessage, QuitMessage> messageHub;
    auto sender = messageHub.getSender();
    std::size_t counter = 0;

    std::thread workerThread([&]()
    {
         messageHub.subscribe<TestMessage>([&](auto&& message)
        {
            ++message.counter;
            message.processed.store(true, std::memory_order::release);
            return true;
        });

        messageHub.subscribe<QuitMessage>([&](auto&& message)
        {
            return false;
        });

        while(messageHub.wait())
        {}
    });

    std::atomic<bool> processed;

    for (auto _ : state)
    {
        processed.store(false, std::memory_order::release);

        sender.send(TestMessage(counter, processed));

        bool expected = true;
        while(!processed.compare_exchange_strong(expected, false, std::memory_order::acq_rel))
        {
            expected = true;
        }
    }

    sender.send(QuitMessage{});
    workerThread.join();
}
BENCHMARK(BM_sendMessage);

static void BM_boudedQueue(benchmark::State& state) {
    auto queue = dory::concurrency::BoundedQueue<int, 3>();
    for (auto _ : state)
    {
        queue.push(1);
        queue.pop();
    }
}
BENCHMARK(BM_boudedQueue);

BENCHMARK_MAIN();