#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <allocatorBuilder.h>
#include <backward.hpp>

#include <dory/data-structures/containers/lockfree/freeListArray.h>

#include "assertUtils.h"

TEST(FreeListTests, basic)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
    using AllocatorType = typename decltype(allocator)::element_type;

    auto list = dory::data_structures::containers::lockfree::freelist::FreeListArray<int, AllocatorType> { *allocator };

    std::cout << "Free list" << std::endl;
    list.printFreeList();

    const auto id1 = list.add(1);
    const auto id2 = list.add(2);

    EXPECT_EQ(list.size(), 2);

    EXPECT_EQ(list.get(id1), 1);
    EXPECT_EQ(list.get(id2), 2);

    std::cout << "Add items 1, 2" << std::endl;
    list.print();
    std::cout << "Free list" << std::endl;
    list.printFreeList();

    list.retire(id1);
    list.retire(id1);

    list.reclaim();
    std::cout << "Delete item 1" << std::endl;
    list.print();
    std::cout << "Free list" << std::endl;
    list.printFreeList();
    EXPECT_EQ(list.size(), 1);

    const auto id3 = list.add(3);
    EXPECT_EQ(id3.index, id1.index); //item 3 has to be added to the previous slot of item 1
    std::cout << "Add item 3" << std::endl;
    list.print();
    std::cout << "Free list" << std::endl;
    list.printFreeList();

    const auto id4 = list.add(4);
    std::cout << "Add item 4" << std::endl;
    list.print();
    std::cout << "Free list" << std::endl;
    list.printFreeList();

    list.retire(id2);
    list.retire(id3);
    list.retire(id4);
    list.reclaim();
    std::cout << "Delete items 2, 3, 4" << std::endl;
    list.print();
    std::cout << "Free list" << std::endl;
    list.sortFreeList();
    list.printFreeList();
    EXPECT_EQ(list.size(), 0);
}

TEST(FreeListTests, sorting)
{
    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
    using AllocatorType = typename decltype(allocator)::element_type;
    using ListType = dory::data_structures::containers::lockfree::freelist::FreeListArray<int, AllocatorType, 16>;
    using SlotType = ListType::SlotType;

    auto list = ListType { *allocator };
    auto freeListState = dory::data_structures::containers::lockfree::freelist::FreeListState<ListType::SlotIndexType, ListType::UNDEFINED_HEAD_INDEX> {};

    for(std::size_t i = 0; i != 11; ++i)
    {
        list.reserveSlot();
    }

    freeListState.head.store(10);

    SlotType* slot = list.getSlot(10);
    slot->nextSlot.store(3);

    slot = list.getSlot(3);
    slot->nextSlot.store(8);

    slot = list.getSlot(8);
    slot->nextSlot.store(2);

    slot = list.getSlot(2);
    slot->nextSlot.store(7);

    slot = list.getSlot(7);
    slot->nextSlot.store(ListType::UNDEFINED_HEAD_INDEX);

    dory::data_structures::containers::lockfree::freelist::sort(freeListState, list);

    std::vector<ListType::SlotIndexType> freeListIndices;
    ListType::SlotIndexType index = freeListState.head.load();
    while(index != ListType::UNDEFINED_HEAD_INDEX)
    {
        freeListIndices.push_back(index);
        slot = list.getSlot(index);
        index = slot->nextSlot.load();
    }

    dory::test_utilities::assertList(freeListIndices, { 2, 3, 7, 8, 10 });
}

#include <barrier>
#include <thread>
#include <vector>
#include <random>

// ---- Payload stored in the list (easy to verify) ----
struct Payload
{
    uint32_t tid;
    uint32_t seq;
    uint64_t cookie;

    bool operator==(const Payload& o) const noexcept
    {
        return tid == o.tid && seq == o.seq && cookie == o.cookie;
    }
};

// Helper to make deterministic-ish cookies
static inline uint64_t make_cookie(uint32_t tid, uint32_t seq) noexcept
{
    uint64_t x = (static_cast<uint64_t>(tid) << 32) ^ static_cast<uint64_t>(seq);
    x ^= 0x9E3779B97F4A7C15ULL;
    x *= 0xD1B54A32D192ED03ULL;
    x ^= (x >> 33);
    x *= 0x94D049BB133111EBULL;
    x ^= (x >> 33);
    return x;
}

void printStack()
{
    namespace bw = backward;

    bw::StackTrace st;
    st.load_here(64);  // capture up to 64 frames

    bw::TraceResolver resolver;
    resolver.load_stacktrace(st);

    for (int i = static_cast<int>(st.size()) - 1; i >= 0; --i)
    {
        const bw::ResolvedTrace trace = resolver.resolve(st[i]);
        std::cout << "#" << i << " ";

        if (!trace.source.filename.empty())
        {
            std::cout << trace.source.filename << ":"
                      << trace.source.line << " - ";
        }

        std::cout << trace.object_function << std::endl;
    }
}

TEST(FreeListArray, Stress_AddRetire_6Threads_Barrier)
{
    dory::assert::Assert::assertFailureHandler = [](const char * msg) {
        std::cerr << msg << std::endl;
    };

    dory::assert::InHouseAssert::assertFailureHandler = [](const char * msg) {
        std::cerr << msg << std::endl;
        doryDebugBreak();
    };

    dory::assert::DebugAssert::assertFailureHandler = [](const char * msg) {
        std::cerr << msg << std::endl;
        printStack();
        doryDebugBreak();
        std::exit(EXIT_FAILURE);
    };

    constexpr int kThreads = 6;
    constexpr int kOpsPerThread = 50'000;   // adjust up/down
    constexpr int kRetireChancePct = 40;    // retire probability each op
    constexpr int kFinalRetireChancePct = 20;

    dory::test_utilities::AllocatorBuilder allocBuilder;
    const auto allocator = allocBuilder.build();
    using AllocatorType = typename decltype(allocator)::element_type;
    using ListType = dory::data_structures::containers::lockfree::freelist::FreeListArray<Payload, AllocatorType, 32, 1048576>;

    auto list = std::make_shared<ListType>(*allocator);

    // ---- Per-thread logs (no contention) ----
    using Id = ListType::SlotIdentifier;

    struct ThreadLog
    {
        std::vector<Id> live_ids;
        std::vector<Payload> live_payloads;
        std::vector<Id> retired_ids;

        void reserve(std::size_t n)
        {
            live_ids.reserve(n);
            live_payloads.reserve(n);
            retired_ids.reserve(n);
        }
    };

    std::vector<ThreadLog> logs(kThreads);
    for (auto& l : logs) l.reserve(kOpsPerThread / 2);

    // Barrier: workers + main thread (so main can release them)
    std::barrier start_barrier(kThreads + 1);

    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    for (int t = 0; t < kThreads; ++t)
    {
        threads.emplace_back([&, t]()
        {
            // Deterministic per-thread RNG
            std::mt19937_64 rng(0xC0FFEEULL + static_cast<uint64_t>(t) * 0x9E3779B97F4A7C15ULL);
            std::uniform_int_distribution<int> coin(0, 99);

            auto& log = logs[t];

            // Wait for synchronized start
            start_barrier.arrive_and_wait();

            for (uint32_t i = 0; i < static_cast<uint32_t>(kOpsPerThread); ++i)
            {
                Payload p{ static_cast<uint32_t>(t), i, make_cookie(static_cast<uint32_t>(t), i) };

                Id id = list->add(p);
                log.live_ids.push_back(id);
                log.live_payloads.push_back(p);

                // Random retire of a previously-live entry
                if (!log.live_ids.empty() && coin(rng) < kRetireChancePct)
                {
                    std::uniform_int_distribution<std::size_t> pick(0, log.live_ids.size() - 1);
                    std::size_t k = pick(rng);

                    Id rid = log.live_ids[k];
                    list->retire(rid);
                    log.retired_ids.push_back(rid);

                    // swap-remove from live lists
                    log.live_ids[k] = log.live_ids.back();
                    log.live_ids.pop_back();
                    log.live_payloads[k] = log.live_payloads.back();
                    log.live_payloads.pop_back();
                }
            }

            // Retire some remaining live to increase churn
            for (std::size_t k = 0; k < log.live_ids.size(); )
            {
                if (coin(rng) < kFinalRetireChancePct)
                {
                    Id rid = log.live_ids[k];
                    list->retire(rid);
                    log.retired_ids.push_back(rid);

                    log.live_ids[k] = log.live_ids.back();
                    log.live_ids.pop_back();
                    log.live_payloads[k] = log.live_payloads.back();
                    log.live_payloads.pop_back();
                }
                else
                {
                    ++k;
                }
            }
        });
    }

    // Release all worker threads simultaneously
    start_barrier.arrive_and_wait();

    for (auto& th : threads) th.join();

    // Coalescing: reclaim only after workers finished
    list->reclaim();

    // ---- Validate expected live items ----
    for (int t = 0; t < kThreads; ++t)
    {
        const auto& log = logs[t];
        ASSERT_EQ(log.live_ids.size(), log.live_payloads.size());

        for (std::size_t i = 0; i < log.live_ids.size(); ++i)
        {
            const Id& id = log.live_ids[i];
            const Payload& expected = log.live_payloads[i];

            Payload& got = list->get(id);

            ASSERT_EQ(got.tid, expected.tid) << "tid mismatch; possible freelist corruption / slot reuse bug";
            ASSERT_EQ(got.seq, expected.seq) << "seq mismatch; possible stale-key retire / generation bug";
            ASSERT_EQ(got.cookie, expected.cookie) << "cookie mismatch; possible duplicate allocation or overwrite";
        }
    }
}