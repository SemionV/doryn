#include <benchmark/benchmark.h>

#include <dory/concurrency/queue.h>

#include <dory/concurrency/log.h>
#include <dory/concurrency/messaging.h>
#include <dory/engine/testing/dataGenerators.h>
#include <dory/engine/testing/quickSort.h>
#include <dory/engine/testing/atm_book.h>

static const constexpr std::size_t largeDataCount = 10000;
static const constexpr std::size_t smallDataCount = 10;

static void BM_stdSort(benchmark::State& state) {
    auto data = dory::testing::getArray<int, smallDataCount>();
    for (auto _ : state)
    {
        auto dataCopy = *data;
        std::ranges::sort(dataCopy, std::ranges::less());
    }
}
BENCHMARK(BM_stdSort);

static void BM_stdSortLarge(benchmark::State& state) {
    auto data = dory::testing::getArray<int, largeDataCount>();
    for (auto _ : state)
    {
        auto dataCopy = *data;
        std::ranges::sort(dataCopy, std::ranges::less());
    }
}
BENCHMARK(BM_stdSortLarge);

static void BM_quickSort(benchmark::State& state) {
    auto data = dory::testing::getList<int, smallDataCount>();
    for (auto _ : state)
    {
        dory::testing::quickSort(data);
    }
}
BENCHMARK(BM_quickSort);

static void BM_sendMessage_book(benchmark::State& state) {
    using Log = dory::concurrency::logging::Log<decltype(std::cout)>;
    auto log = Log(std::cout);

    std::atomic<bool> processed = false;
    std::size_t counter = 0;
    dory::testing::atm_book::test_machine machine(log);
    std::thread machine_thread(&dory::testing::atm_book::test_machine<Log>::run, &machine);

    auto machine_queue(machine.get_sender());

    for (auto _ : state)
    {
        processed.store(false, std::memory_order::release);

        machine_queue.send(dory::testing::atm_book::test_message(counter, processed));

        bool expected = true;
        while(!processed.compare_exchange_strong(expected, false, std::memory_order::acq_rel))
        {
            expected = true;
        }
    }

    machine.done();
    machine_thread.join();
}
BENCHMARK(BM_sendMessage_book);

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