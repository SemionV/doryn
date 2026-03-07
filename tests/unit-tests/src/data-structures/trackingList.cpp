#include <gtest/gtest.h>

#include <atomic>
#include <algorithm>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <utility>
#include <type_traits>

#include <dory/data-structures/containers/lockfree/trackingList.h>

namespace dory::data_structures::containers::lockfree::tests
{
    //--------------------------------------------------------------------------
    // Test allocator
    //--------------------------------------------------------------------------

    template<typename T>
    concept HasNextMember = requires(T value)
    {
        value.next;
    };

    class TestAllocator
    {
    public:
        std::atomic<int> allocations = 0;
        std::atomic<int> deallocations = 0;

        template<typename TObject, typename TLabel, typename... TArgs>
        TObject* allocateObject(TLabel, TArgs&&... args)
        {
            TObject* object = new TObject(std::forward<TArgs>(args)...);
            allocations.fetch_add(1, std::memory_order_relaxed);
            return object;
        }

        template<typename TObject>
        void deallocateObject(TObject* object)
        {
            delete object;
            deallocations.fetch_add(1, std::memory_order_relaxed);
        }
    };

    //--------------------------------------------------------------------------
    // Helpers
    //--------------------------------------------------------------------------

    constexpr LabelType kTestLabel = static_cast<LabelType>(0);

    template<typename T>
    using TrackingListType = dory::data_structures::containers::lockfree::TrackingList<T, TestAllocator, kTestLabel>;

    //--------------------------------------------------------------------------
    // Tests
    //--------------------------------------------------------------------------

    TEST(TrackingListTests, ClearOnEmptyListDoesNothing)
    {
        TestAllocator allocator;
        TrackingListType<int> list(allocator);

        int callbackCount = 0;
        list.clear([&](int&)
        {
            ++callbackCount;
        });

        EXPECT_EQ(callbackCount, 0);
        EXPECT_EQ(allocator.allocations.load(std::memory_order_relaxed), 0);
        EXPECT_EQ(allocator.deallocations.load(std::memory_order_relaxed), 0);
    }

    TEST(TrackingListTests, PushSingleElementThenClearInvokesCallbackAndDeallocates)
    {
        TestAllocator allocator;
        TrackingListType<int> list(allocator);

        list.push(42);

        std::vector<int> values;
        list.clear([&](int& value)
        {
            values.push_back(value);
        });

        ASSERT_EQ(values.size(), 1u);
        EXPECT_EQ(values[0], 42);
        EXPECT_EQ(allocator.allocations.load(std::memory_order_relaxed), 1);
        EXPECT_EQ(allocator.deallocations.load(std::memory_order_relaxed), 1);
    }

    TEST(TrackingListTests, PushMultipleElementsThenClearDrainsInLifoOrder)
    {
        TestAllocator allocator;
        TrackingListType<int> list(allocator);

        list.push(1);
        list.push(2);
        list.push(3);

        std::vector<int> values;
        list.clear([&](int& value)
        {
            values.push_back(value);
        });

        ASSERT_EQ(values.size(), 3u);
        EXPECT_EQ(values[0], 3);
        EXPECT_EQ(values[1], 2);
        EXPECT_EQ(values[2], 1);

        EXPECT_EQ(allocator.allocations.load(std::memory_order_relaxed), 3);
        EXPECT_EQ(allocator.deallocations.load(std::memory_order_relaxed), 3);
    }

    TEST(TrackingListTests, ClearCanBeCalledTwiceAndSecondCallIsNoOp)
    {
        TestAllocator allocator;
        TrackingListType<int> list(allocator);

        list.push(10);
        list.push(20);

        int callbackCount = 0;
        list.clear([&](int&)
        {
            ++callbackCount;
        });

        list.clear([&](int&)
        {
            ++callbackCount;
        });

        EXPECT_EQ(callbackCount, 2);
        EXPECT_EQ(allocator.allocations.load(std::memory_order_relaxed), 2);
        EXPECT_EQ(allocator.deallocations.load(std::memory_order_relaxed), 2);
    }

    TEST(TrackingListTests, DestructorClearsRemainingNodes)
    {
        TestAllocator allocator;

        {
            TrackingListType<int> list(allocator);
            list.push(7);
            list.push(8);
            list.push(9);
        }

        EXPECT_EQ(allocator.allocations.load(std::memory_order_relaxed), 3);
        EXPECT_EQ(allocator.deallocations.load(std::memory_order_relaxed), 3);
    }

    TEST(TrackingListTests, SupportsMoveOnlyTypes)
    {
        TestAllocator allocator;
        TrackingListType<std::unique_ptr<int>> list(allocator);

        list.push(std::make_unique<int>(11));
        list.push(std::make_unique<int>(22));

        std::vector<int> values;
        list.clear([&](std::unique_ptr<int>& value)
        {
            ASSERT_NE(value, nullptr);
            values.push_back(*value);
        });

        ASSERT_EQ(values.size(), 2u);
        EXPECT_EQ(values[0], 22);
        EXPECT_EQ(values[1], 11);

        EXPECT_EQ(allocator.allocations.load(std::memory_order_relaxed), 2);
        EXPECT_EQ(allocator.deallocations.load(std::memory_order_relaxed), 2);
    }

    TEST(TrackingListTests, ConcurrentPushThenQuiescentClearCollectsAllElements)
    {
        TestAllocator allocator;
        TrackingListType<int> list(allocator);

        constexpr int threadCount = 8;
        constexpr int pushesPerThread = 2000;

        std::vector<std::thread> threads;
        threads.reserve(threadCount);

        for (int threadIndex = 0; threadIndex < threadCount; ++threadIndex)
        {
            threads.emplace_back([&, threadIndex]()
            {
                const int base = threadIndex * pushesPerThread;
                for (int i = 0; i < pushesPerThread; ++i)
                {
                    list.push(base + i);
                }
            });
        }

        for (std::thread& thread : threads)
            thread.join();

        std::vector<int> values;
        values.reserve(threadCount * pushesPerThread);

        list.clear([&](int& value)
        {
            values.push_back(value);
        });

        const int expectedCount = threadCount * pushesPerThread;

        ASSERT_EQ(static_cast<int>(values.size()), expectedCount);
        EXPECT_EQ(allocator.allocations.load(std::memory_order_relaxed), expectedCount);
        EXPECT_EQ(allocator.deallocations.load(std::memory_order_relaxed), expectedCount);

        std::sort(values.begin(), values.end());

        for (int i = 0; i < expectedCount; ++i)
            EXPECT_EQ(values[i], i);
    }

    TEST(TrackingListTests, CallbackCanMutateTrackedObjectsBeforeDestruction)
    {
        struct Payload
        {
            int value;
            bool visited = false;

            explicit Payload(int v)
                : value(v)
            {}
        };

        TestAllocator allocator;
        TrackingListType<Payload> list(allocator);

        list.push(1);
        list.push(2);
        list.push(3);

        int sum = 0;
        int visitedCount = 0;

        list.clear([&](Payload& payload)
        {
            payload.visited = true;
            sum += payload.value;
            if (payload.visited)
                ++visitedCount;
        });

        EXPECT_EQ(sum, 6);
        EXPECT_EQ(visitedCount, 3);
        EXPECT_EQ(allocator.allocations.load(std::memory_order_relaxed), 3);
        EXPECT_EQ(allocator.deallocations.load(std::memory_order_relaxed), 3);
    }

    TEST(TrackingListTests, ReuseAfterClearWorks)
    {
        TestAllocator allocator;
        TrackingListType<int> list(allocator);

        list.push(1);
        list.push(2);

        std::vector<int> firstDrain;
        list.clear([&](int& value)
        {
            firstDrain.push_back(value);
        });

        list.push(5);
        list.push(6);

        std::vector<int> secondDrain;
        list.clear([&](int& value)
        {
            secondDrain.push_back(value);
        });

        ASSERT_EQ(firstDrain.size(), 2u);
        ASSERT_EQ(secondDrain.size(), 2u);

        EXPECT_EQ(firstDrain[0], 2);
        EXPECT_EQ(firstDrain[1], 1);

        EXPECT_EQ(secondDrain[0], 6);
        EXPECT_EQ(secondDrain[1], 5);

        EXPECT_EQ(allocator.allocations.load(std::memory_order_relaxed), 4);
        EXPECT_EQ(allocator.deallocations.load(std::memory_order_relaxed), 4);
    }

    struct Payload
    {
        static inline std::atomic<int> liveCount = 0;
        static inline std::atomic<int> destructorCount = 0;
        static inline std::atomic<int> moveConstructCount = 0;
        static inline std::atomic<int> copyConstructCount = 0;

        int value = 0;

        explicit Payload(int v)
            : value(v)
        {
            liveCount.fetch_add(1, std::memory_order_relaxed);
        }

        Payload(const Payload& other)
            : value(other.value)
        {
            liveCount.fetch_add(1, std::memory_order_relaxed);
            copyConstructCount.fetch_add(1, std::memory_order_relaxed);
        }

        Payload(Payload&& other) noexcept
            : value(other.value)
        {
            liveCount.fetch_add(1, std::memory_order_relaxed);
            moveConstructCount.fetch_add(1, std::memory_order_relaxed);
            other.value = -1;
        }

        Payload& operator=(const Payload&) = default;
        Payload& operator=(Payload&&) noexcept = default;

        ~Payload()
        {
            destructorCount.fetch_add(1, std::memory_order_relaxed);
            liveCount.fetch_sub(1, std::memory_order_relaxed);
        }

        static void resetCounters()
        {
            liveCount.store(0, std::memory_order_relaxed);
            destructorCount.store(0, std::memory_order_relaxed);
            moveConstructCount.store(0, std::memory_order_relaxed);
            copyConstructCount.store(0, std::memory_order_relaxed);
        }
    };

    TEST(TrackingListTests, PayloadLifetimeIsHandledCorrectly)
    {
        Payload::resetCounters();

        TestAllocator allocator;

        {
            TrackingListType<Payload> list(allocator);

            list.push(1);
            list.push(2);
            list.push(3);

            EXPECT_EQ(Payload::liveCount.load(std::memory_order_relaxed), 3);

            int sum = 0;
            int visitedCount = 0;

            list.clear([&](Payload& payload)
            {
                sum += payload.value;
                ++visitedCount;
            });

            EXPECT_EQ(sum, 6);
            EXPECT_EQ(visitedCount, 3);
            EXPECT_EQ(Payload::liveCount.load(std::memory_order_relaxed), 0);
            EXPECT_EQ(Payload::destructorCount.load(std::memory_order_relaxed), 3);
        }

        EXPECT_EQ(Payload::liveCount.load(std::memory_order_relaxed), 0);
        EXPECT_EQ(Payload::destructorCount.load(std::memory_order_relaxed), 3);
        EXPECT_EQ(allocator.allocations.load(std::memory_order_relaxed), 3);
        EXPECT_EQ(allocator.deallocations.load(std::memory_order_relaxed), 3);
    }

    TEST(TrackingListTests, StressRepeatedConcurrentPushAndClear)
    {
        TestAllocator allocator;
        TrackingListType<int> list(allocator);

        constexpr int roundCount = 100;
        constexpr int threadCount = 6;
        constexpr int pushesPerThread = 500;

        for (int round = 0; round < roundCount; ++round)
        {
            std::vector<std::thread> threads;
            threads.reserve(threadCount);

            for (int threadIndex = 0; threadIndex < threadCount; ++threadIndex)
            {
                threads.emplace_back([&, round, threadIndex]()
                {
                    const int base = round * threadCount * pushesPerThread +
                                     threadIndex * pushesPerThread;

                    for (int i = 0; i < pushesPerThread; ++i)
                    {
                        list.push(base + i);
                    }
                });
            }

            for (std::thread& thread : threads)
                thread.join();

            std::vector<int> values;
            values.reserve(threadCount * pushesPerThread);

            list.clear([&](int& value)
            {
                values.push_back(value);
            });

            const int expectedCount = threadCount * pushesPerThread;
            ASSERT_EQ(static_cast<int>(values.size()), expectedCount);

            std::sort(values.begin(), values.end());

            const int expectedBase = round * threadCount * pushesPerThread;
            for (int i = 0; i < expectedCount; ++i)
            {
                EXPECT_EQ(values[i], expectedBase + i);
            }
        }

        const int totalExpected = roundCount * threadCount * pushesPerThread;
        EXPECT_EQ(allocator.allocations.load(std::memory_order_relaxed), totalExpected);
        EXPECT_EQ(allocator.deallocations.load(std::memory_order_relaxed), totalExpected);
    }
}