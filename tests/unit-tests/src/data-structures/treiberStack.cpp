#include <barrier>
#include <gtest/gtest.h>
#include <thread>

#include <dory/data-structures/containers/lockfree/stack.h>
#include <dory/memory/allocators/general/systemAllocator.h>
#include <dory/data-structures/memory-reclamation/hazardPointers.h>

namespace dory::data_structures::containers::lockfree::tests
{
    using TestAllocator = memory::allocators::general::SystemAllocator;
    using HazardPointerDomainType = memory_reclamation::hazard_pointers::Domain<8, 2, 128>;

    template <typename T>
    using Stack = HazardTreiberStack<T, HazardPointerDomainType, TestAllocator>;

    // ---------------------------------------------------------------------
    // Tests
    // ---------------------------------------------------------------------

    TEST(HazardTreiberStackTest, NewStackIsEmpty)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<int> stack(domain, allocator);

        EXPECT_TRUE(stack.empty());
    }

    TEST(HazardTreiberStackTest, PopOnEmptyStackReturnsNullopt)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<int> stack(domain, allocator);

        auto result = stack.pop(static_cast<ThreadId>(0));

        EXPECT_FALSE(result.has_value());
        EXPECT_TRUE(stack.empty());
    }

    TEST(HazardTreiberStackTest, PushThenPopSingleElement)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<int> stack(domain, allocator);

        stack.push(42);

        EXPECT_FALSE(stack.empty());

        auto result = stack.pop(static_cast<ThreadId>(0));

        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(*result, 42);
        EXPECT_TRUE(stack.empty());
    }

    TEST(HazardTreiberStackTest, PopFollowsLifoOrder)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<int> stack(domain, allocator);

        stack.push(1);
        stack.push(2);
        stack.push(3);

        auto a = stack.pop(static_cast<ThreadId>(0));
        auto b = stack.pop(static_cast<ThreadId>(0));
        auto c = stack.pop(static_cast<ThreadId>(0));
        auto d = stack.pop(static_cast<ThreadId>(0));

        ASSERT_TRUE(a.has_value());
        ASSERT_TRUE(b.has_value());
        ASSERT_TRUE(c.has_value());

        EXPECT_EQ(*a, 3);
        EXPECT_EQ(*b, 2);
        EXPECT_EQ(*c, 1);
        EXPECT_FALSE(d.has_value());
        EXPECT_TRUE(stack.empty());
    }

    TEST(HazardTreiberStackTest, InterleavedPushAndPopWorksCorrectly)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<int> stack(domain, allocator);

        stack.push(10);
        stack.push(20);

        auto a = stack.pop(static_cast<ThreadId>(0));
        ASSERT_TRUE(a.has_value());
        EXPECT_EQ(*a, 20);

        stack.push(30);

        auto b = stack.pop(static_cast<ThreadId>(0));
        auto c = stack.pop(static_cast<ThreadId>(0));
        auto d = stack.pop(static_cast<ThreadId>(0));

        ASSERT_TRUE(b.has_value());
        ASSERT_TRUE(c.has_value());

        EXPECT_EQ(*b, 30);
        EXPECT_EQ(*c, 10);
        EXPECT_FALSE(d.has_value());
        EXPECT_TRUE(stack.empty());
    }

    TEST(HazardTreiberStackTest, SupportsMoveOnlyTypes)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<std::unique_ptr<int>> stack(domain, allocator);

        stack.push(std::make_unique<int>(1337));

        auto result = stack.pop(static_cast<ThreadId>(0));

        ASSERT_TRUE(result.has_value());
        ASSERT_TRUE(*result);
        EXPECT_EQ(**result, 1337);
        EXPECT_TRUE(stack.empty());
    }

    TEST(HazardTreiberStackTest, MultipleProducersSingleConsumerAllValuesAreRecovered)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<int> stack(domain, allocator);

        constexpr int producerCount = 4;
        constexpr int itemsPerProducer = 1000;
        constexpr int totalItems = producerCount * itemsPerProducer;

        std::barrier startBarrier(producerCount + 1);
        std::vector<std::thread> producers;
        producers.reserve(producerCount);

        for (int p = 0; p < producerCount; ++p)
        {
            producers.emplace_back([&, p]
            {
                startBarrier.arrive_and_wait();

                const int base = p * itemsPerProducer;
                for (int i = 0; i < itemsPerProducer; ++i)
                    stack.push(base + i);
            });
        }

        startBarrier.arrive_and_wait();

        for (auto& t : producers)
            t.join();

        std::vector<int> popped;
        popped.reserve(totalItems);

        for (;;)
        {
            auto value = stack.pop(static_cast<ThreadId>(0));
            if (!value.has_value())
                break;

            popped.push_back(*value);
        }

        ASSERT_EQ(static_cast<int>(popped.size()), totalItems);

        std::ranges::sort(popped);
        for (int i = 0; i < totalItems; ++i)
            EXPECT_EQ(popped[i], i);

        EXPECT_TRUE(stack.empty());
    }

    TEST(HazardTreiberStackTest, MultipleProducersMultipleConsumersRecoverEveryValueExactlyOnce)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<int> stack(domain, allocator);

        constexpr int producerCount = 4;
        constexpr int consumerCount = 4;
        constexpr int itemsPerProducer = 2000;
        constexpr int totalItems = producerCount * itemsPerProducer;

        std::barrier startBarrier(producerCount + consumerCount);
        std::atomic<int> consumedCount { 0 };

        std::vector<std::thread> producers;
        std::vector<std::thread> consumers;
        producers.reserve(producerCount);
        consumers.reserve(consumerCount);

        std::vector<std::vector<int>> consumed(consumerCount);

        for (int p = 0; p < producerCount; ++p)
        {
            producers.emplace_back([&, p]
            {
                startBarrier.arrive_and_wait();

                const int base = p * itemsPerProducer;
                const auto tid = static_cast<ThreadId>(p);

                for (int i = 0; i < itemsPerProducer; ++i)
                    stack.push(base + i);

                (void)tid; // push() does not use thread id
            });
        }

        for (int c = 0; c < consumerCount; ++c)
        {
            consumers.emplace_back([&, c]
            {
                const auto tid = static_cast<ThreadId>(producerCount + c);
                startBarrier.arrive_and_wait();

                auto& local = consumed[c];
                local.reserve(totalItems / consumerCount + 16);

                while (consumedCount.load(std::memory_order_acquire) < totalItems)
                {
                    auto value = stack.pop(tid);
                    if (!value.has_value())
                    {
                        std::this_thread::yield();
                        continue;
                    }

                    const int prev = consumedCount.fetch_add(1, std::memory_order_acq_rel);
                    if (prev < totalItems)
                        local.push_back(*value);
                }
            });
        }

        for (auto& t : producers)
            t.join();

        for (auto& t : consumers)
            t.join();

        std::vector<int> allValues;
        allValues.reserve(totalItems);

        for (auto& local : consumed)
        {
            allValues.insert(allValues.end(), local.begin(), local.end());
        }

        ASSERT_EQ(static_cast<int>(allValues.size()), totalItems);

        std::ranges::sort(allValues);
        for (int i = 0; i < totalItems; ++i)
            EXPECT_EQ(allValues[i], i);

        EXPECT_TRUE(stack.empty());
    }

    // ---------------------------------------------------------------------
    // Payload types
    // ---------------------------------------------------------------------

    struct LargePayload
    {
        int id {};
        std::array<std::uint64_t, 128> words {};
        std::string name;

        LargePayload() = default;

        explicit LargePayload(int value) :
            id(value),
            name("payload-" + std::to_string(value))
        {
            for (std::size_t i = 0; i < words.size(); ++i)
                words[i] = static_cast<std::uint64_t>(value) * 1000ull + i;
        }

        [[nodiscard]] bool isValid() const noexcept
        {
            if (name != "payload-" + std::to_string(id))
                return false;

            for (std::size_t i = 0; i < words.size(); ++i)
            {
                if (words[i] != static_cast<std::uint64_t>(id) * 1000ull + i)
                    return false;
            }

            return true;
        }
    };

    struct MoveOnlyPayload
    {
        std::unique_ptr<int> value;
        std::unique_ptr<std::string> text;

        explicit MoveOnlyPayload(int v) :
            value(std::make_unique<int>(v)),
            text(std::make_unique<std::string>("value-" + std::to_string(v)))
        {}

        MoveOnlyPayload(MoveOnlyPayload&&) noexcept = default;
        MoveOnlyPayload& operator=(MoveOnlyPayload&&) noexcept = default;

        MoveOnlyPayload(const MoveOnlyPayload&) = delete;
        MoveOnlyPayload& operator=(const MoveOnlyPayload&) = delete;
    };

    struct LifetimeTrackedPayload
    {
        static inline std::atomic<int> ctorCount { 0 };
        static inline std::atomic<int> dtorCount { 0 };
        static inline std::atomic<int> moveCount { 0 };

        int value {};

        explicit LifetimeTrackedPayload(int v) :
            value(v)
        {
            ctorCount.fetch_add(1, std::memory_order_relaxed);
        }

        LifetimeTrackedPayload(LifetimeTrackedPayload&& other) noexcept :
            value(other.value)
        {
            moveCount.fetch_add(1, std::memory_order_relaxed);
        }

        LifetimeTrackedPayload& operator=(LifetimeTrackedPayload&& other) noexcept
        {
            value = other.value;
            moveCount.fetch_add(1, std::memory_order_relaxed);
            return *this;
        }

        LifetimeTrackedPayload(const LifetimeTrackedPayload&) = delete;
        LifetimeTrackedPayload& operator=(const LifetimeTrackedPayload&) = delete;

        ~LifetimeTrackedPayload()
        {
            dtorCount.fetch_add(1, std::memory_order_relaxed);
        }

        static void resetCounters() noexcept
        {
            ctorCount.store(0, std::memory_order_relaxed);
            dtorCount.store(0, std::memory_order_relaxed);
            moveCount.store(0, std::memory_order_relaxed);
        }
    };

    struct LiveTrackedPayload
    {
        static inline std::atomic<int> liveCount { 0 };

        int value {};

        explicit LiveTrackedPayload(int v) :
            value(v)
        {
            liveCount.fetch_add(1, std::memory_order_relaxed);
        }

        LiveTrackedPayload(LiveTrackedPayload&& other) noexcept :
            value(other.value)
        {
            liveCount.fetch_add(1, std::memory_order_relaxed);
        }

        LiveTrackedPayload& operator=(LiveTrackedPayload&& other) noexcept
        {
            value = other.value;
            return *this;
        }

        LiveTrackedPayload(const LiveTrackedPayload&) = delete;
        LiveTrackedPayload& operator=(const LiveTrackedPayload&) = delete;

        ~LiveTrackedPayload()
        {
            liveCount.fetch_sub(1, std::memory_order_relaxed);
        }

        static void reset() noexcept
        {
            liveCount.store(0, std::memory_order_relaxed);
        }
    };

    // ---------------------------------------------------------------------
    // Payload tests
    // ---------------------------------------------------------------------

    TEST(HazardTreiberStackPayloadTest, LargePayloadRoundTripsWithoutCorruption)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<LargePayload> stack(domain, allocator);

        for (int i = 0; i < 100; ++i)
            stack.push(LargePayload{i});

        for (int expected = 99; expected >= 0; --expected)
        {
            auto result = stack.pop(static_cast<ThreadId>(0));
            ASSERT_TRUE(result.has_value());
            EXPECT_EQ(result->id, expected);
            EXPECT_TRUE(result->isValid());
        }

        EXPECT_FALSE(stack.pop(static_cast<ThreadId>(0)).has_value());
        EXPECT_TRUE(stack.empty());
    }

    TEST(HazardTreiberStackPayloadTest, MoveOnlyPayloadRoundTripsCorrectly)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<MoveOnlyPayload> stack(domain, allocator);

        stack.push(MoveOnlyPayload{7});
        stack.push(MoveOnlyPayload{11});

        auto a = stack.pop(static_cast<ThreadId>(0));
        auto b = stack.pop(static_cast<ThreadId>(0));

        ASSERT_TRUE(a.has_value());
        ASSERT_TRUE(b.has_value());

        ASSERT_TRUE(a->value);
        ASSERT_TRUE(a->text);
        ASSERT_TRUE(b->value);
        ASSERT_TRUE(b->text);

        EXPECT_EQ(*a->value, 11);
        EXPECT_EQ(*a->text, "value-11");
        EXPECT_EQ(*b->value, 7);
        EXPECT_EQ(*b->text, "value-7");

        EXPECT_TRUE(stack.empty());
    }

    TEST(HazardTreiberStackPayloadTest, NoPayloadInstancesRemainAfterDrain)
    {
        LiveTrackedPayload::reset();

        {
            HazardPointerDomainType domain;
            TestAllocator allocator;
            Stack<LiveTrackedPayload> stack(domain, allocator);

            constexpr int count = 100;

            for (int i = 0; i < count; ++i)
                stack.push(LiveTrackedPayload{i});

            for (int i = 0; i < count; ++i)
            {
                auto result = stack.pop(static_cast<ThreadId>(0));
                ASSERT_TRUE(result.has_value());
                EXPECT_EQ(result->value, count - 1 - i);
            }

            EXPECT_TRUE(stack.empty());
        }

        EXPECT_EQ(LiveTrackedPayload::liveCount.load(std::memory_order_relaxed), 0);
    }

    TEST(HazardTreiberStackPayloadTest, LargePayloadConcurrentProducerConsumerIntegrity)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<LargePayload> stack(domain, allocator);

        constexpr int producerCount = 4;
        constexpr int consumerCount = 4;
        constexpr int itemsPerProducer = 500;
        constexpr int totalItems = producerCount * itemsPerProducer;

        std::atomic<int> produced { 0 };
        std::atomic<int> consumed { 0 };

        std::vector<std::thread> producers;
        std::vector<std::thread> consumers;
        std::vector<std::vector<int>> results(consumerCount);

        producers.reserve(producerCount);
        consumers.reserve(consumerCount);

        for (int p = 0; p < producerCount; ++p)
        {
            producers.emplace_back([&, p]
            {
                for (int i = 0; i < itemsPerProducer; ++i)
                {
                    int id = p * itemsPerProducer + i;
                    stack.push(LargePayload{id});
                    produced.fetch_add(1, std::memory_order_release);
                }
            });
        }

        for (int c = 0; c < consumerCount; ++c)
        {
            consumers.emplace_back([&, c]
            {
                auto& local = results[c];

                for (;;)
                {
                    int alreadyConsumed = consumed.load(std::memory_order_acquire);
                    if (alreadyConsumed >= totalItems &&
                        produced.load(std::memory_order_acquire) >= totalItems)
                    {
                        break;
                    }

                    auto value = stack.pop(static_cast<ThreadId>(c));
                    if (!value.has_value())
                    {
                        std::this_thread::yield();
                        continue;
                    }

                    ASSERT_TRUE(value->isValid());
                    local.push_back(value->id);
                    consumed.fetch_add(1, std::memory_order_release);
                }
            });
        }

        for (auto& t : producers)
            t.join();

        while (consumed.load(std::memory_order_acquire) < totalItems)
            std::this_thread::yield();

        for (auto& t : consumers)
            t.join();

        std::vector<int> all;
        all.reserve(totalItems);

        for (auto& local : results)
            all.insert(all.end(), local.begin(), local.end());

        ASSERT_EQ(static_cast<int>(all.size()), totalItems);

        std::ranges::sort(all);
        for (int i = 0; i < totalItems; ++i)
            EXPECT_EQ(all[i], i);

        EXPECT_TRUE(stack.empty());
    }

    // ---------------------------------------------------------------------
    // Stress tests
    // ---------------------------------------------------------------------

    TEST(HazardTreiberStackStressTest, HighContentionBalancedPushPop)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<int> stack(domain, allocator);

        constexpr int threadCount = 8;
        constexpr int opsPerThread = 50000;

        std::atomic<int> pushCounter { 0 };
        std::atomic<int> popCounter { 0 };
        std::atomic<bool> start { false };

        std::vector<std::thread> threads;
        threads.reserve(threadCount);

        for (int t = 0; t < threadCount; ++t)
        {
            threads.emplace_back([&, t]
            {
                while (!start.load(std::memory_order_acquire))
                    std::this_thread::yield();

                const auto tid = static_cast<ThreadId>(t);

                for (int i = 0; i < opsPerThread; ++i)
                {
                    if ((i + t) % 2 == 0)
                    {
                        int value = pushCounter.fetch_add(1, std::memory_order_relaxed);
                        stack.push(value);
                    }
                    else
                    {
                        auto result = stack.pop(tid);
                        if (result.has_value())
                            popCounter.fetch_add(1, std::memory_order_relaxed);
                    }
                }
            });
        }

        start.store(true, std::memory_order_release);

        for (auto& t : threads)
            t.join();

        int drained = 0;
        for (;;)
        {
            auto result = stack.pop(static_cast<ThreadId>(0));
            if (!result.has_value())
                break;
            ++drained;
        }

        const int totalPushed = pushCounter.load(std::memory_order_relaxed);
        const int totalPopped = popCounter.load(std::memory_order_relaxed) + drained;

        EXPECT_EQ(totalPushed, totalPopped);
        EXPECT_TRUE(stack.empty());
    }

    TEST(HazardTreiberStackStressTest, ManyProducersManyConsumersUniqueValuesNoLossNoDuplication)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<int> stack(domain, allocator);

        constexpr int producerCount = 8;
        constexpr int consumerCount = 8;
        constexpr int itemsPerProducer = 20000;
        constexpr int totalItems = producerCount * itemsPerProducer;

        std::atomic<int> consumedCount { 0 };
        std::atomic<bool> producersDone { false };

        std::vector<std::thread> producers;
        std::vector<std::thread> consumers;
        std::vector<std::vector<int>> consumed(consumerCount);

        producers.reserve(producerCount);
        consumers.reserve(consumerCount);

        for (int p = 0; p < producerCount; ++p)
        {
            producers.emplace_back([&, p]
            {
                for (int i = 0; i < itemsPerProducer; ++i)
                {
                    const int value = p * itemsPerProducer + i;
                    stack.push(value);
                }
            });
        }

        for (int c = 0; c < consumerCount; ++c)
        {
            consumers.emplace_back([&, c]
            {
                auto& local = consumed[c];
                local.reserve(totalItems / consumerCount + 128);

                const auto tid = static_cast<ThreadId>(c);

                for (;;)
                {
                    int current = consumedCount.load(std::memory_order_acquire);
                    if (current >= totalItems && producersDone.load(std::memory_order_acquire))
                        break;

                    auto value = stack.pop(tid);
                    if (!value.has_value())
                    {
                        if (producersDone.load(std::memory_order_acquire) &&
                            consumedCount.load(std::memory_order_acquire) >= totalItems)
                        {
                            break;
                        }

                        std::this_thread::yield();
                        continue;
                    }

                    local.push_back(*value);
                    consumedCount.fetch_add(1, std::memory_order_release);
                }
            });
        }

        for (auto& t : producers)
            t.join();

        producersDone.store(true, std::memory_order_release);

        while (consumedCount.load(std::memory_order_acquire) < totalItems)
            std::this_thread::yield();

        for (auto& t : consumers)
            t.join();

        std::vector<int> all;
        all.reserve(totalItems);

        for (auto& local : consumed)
            all.insert(all.end(), local.begin(), local.end());

        ASSERT_EQ(static_cast<int>(all.size()), totalItems);

        std::ranges::sort(all);
        for (int i = 0; i < totalItems; ++i)
            ASSERT_EQ(all[i], i);

        EXPECT_TRUE(stack.empty());
    }

    TEST(HazardTreiberStackStressTest, RepeatedFillAndDrainCycles)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<int> stack(domain, allocator);

        constexpr int cycles = 100;
        constexpr int itemsPerCycle = 5000;

        for (int cycle = 0; cycle < cycles; ++cycle)
        {
            std::vector<std::thread> producers;
            producers.reserve(4);

            for (int p = 0; p < 4; ++p)
            {
                producers.emplace_back([&, p]
                {
                    for (int i = 0; i < itemsPerCycle; ++i)
                        stack.push(cycle * 1000000 + p * itemsPerCycle + i);
                });
            }

            for (auto& t : producers)
                t.join();

            int popped = 0;
            for (;;)
            {
                auto result = stack.pop(static_cast<ThreadId>(0));
                if (!result.has_value())
                    break;
                ++popped;
            }

            EXPECT_EQ(popped, 4 * itemsPerCycle);
            EXPECT_TRUE(stack.empty());
        }
    }

    TEST(HazardTreiberStackStressTest, SoakTestForFixedDuration)
    {
        HazardPointerDomainType domain;
        TestAllocator allocator;
        Stack<int> stack(domain, allocator);

        constexpr int threadCount = 8;
        const auto duration = std::chrono::seconds(2);

        std::atomic<bool> start { false };
        std::atomic<bool> stop { false };
        std::atomic<int> nextValue { 0 };
        std::atomic<int> successfulPops { 0 };

        std::vector<std::thread> threads;
        threads.reserve(threadCount);

        for (int t = 0; t < threadCount; ++t)
        {
            threads.emplace_back([&, t]
            {
                const auto tid = static_cast<ThreadId>(t);

                while (!start.load(std::memory_order_acquire))
                    std::this_thread::yield();

                while (!stop.load(std::memory_order_acquire))
                {
                    if ((t % 2) == 0)
                    {
                        stack.push(nextValue.fetch_add(1, std::memory_order_relaxed));
                    }
                    else
                    {
                        auto result = stack.pop(tid);
                        if (result.has_value())
                            successfulPops.fetch_add(1, std::memory_order_relaxed);
                    }
                }
            });
        }

        start.store(true, std::memory_order_release);
        std::this_thread::sleep_for(duration);
        stop.store(true, std::memory_order_release);

        for (auto& t : threads)
            t.join();

        int drained = 0;
        for (;;)
        {
            auto result = stack.pop(static_cast<ThreadId>(0));
            if (!result.has_value())
                break;
            ++drained;
        }

        const int totalPushed = nextValue.load(std::memory_order_relaxed);
        const int totalPopped = successfulPops.load(std::memory_order_relaxed) + drained;

        EXPECT_EQ(totalPushed, totalPopped);
        EXPECT_TRUE(stack.empty());
    }
}