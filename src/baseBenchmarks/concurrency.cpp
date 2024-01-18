#include "dependencies.h"
#include "base/concurrency/log.h"
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

/*static void BM_quickSortLarge(benchmark::State& state) {
    auto data = dory::testing::getList<int, largeDataCount>();
    for (auto _ : state)
    {
        dory::testing::quickSort(data);
    }
}
BENCHMARK(BM_quickSortLarge);*/

/*static void BM_quickSortLargeParallel(benchmark::State& state) {
    auto data = dory::testing::getList<int, largeDataCount>();
    for (auto _ : state)
    {
        dory::testing::quickSortParallel(data);
    }
}
BENCHMARK(BM_quickSortLargeParallel);*/

static void BM_sendMessage(benchmark::State& state) {
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
BENCHMARK(BM_sendMessage);

BENCHMARK_MAIN();