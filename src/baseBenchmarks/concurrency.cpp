#include "dependencies.h"
#include "base/concurrency/log.h"
#include "base/concurrency/messaging.h"
#include "base/testing/atm_book.h"

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

    std::size_t counter = 0;
    dory::testing::atm_book::test_machine machine(log);
    std::thread machine_thread(&dory::testing::atm_book::test_machine<Log>::run, &machine);

    auto machine_queue(machine.get_sender());

    for (auto _ : state)
    {
        machine_queue.send(dory::testing::atm_book::test_message(counter));
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

    explicit TestMessage(std::size_t& counter):
        counter(counter)
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
            return true;
        });

        messageHub.subscribe<QuitMessage>([&](auto&& message)
        {
            return false;
        });

        while(messageHub.wait())
        {}
    });

    for (auto _ : state)
    {
        sender.send(TestMessage(counter));
    }

    sender.send(QuitMessage{});
    workerThread.join();
}
BENCHMARK(BM_sendMessage);

BENCHMARK_MAIN();