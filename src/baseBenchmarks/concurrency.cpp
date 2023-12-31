#include "dependencies.h"

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

static void BM_quickSortLarge(benchmark::State& state) {
    auto data = dory::testing::getList<int, largeDataCount>();
    for (auto _ : state)
    {
        dory::testing::quickSort(data);
    }
}
BENCHMARK(BM_quickSortLarge);

BENCHMARK_MAIN();