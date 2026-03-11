#include <gtest/gtest.h>
#include <atomic>
#include <utility>
#include <thread>

#include <dory/data-structures/memory-reclamation/epochBasedReclamation.h>
#include <dory/memory/allocators/general/systemAllocator.h>

namespace dory::data_structures::memory_reclamation::ebr::tests
{
    struct CountingJanitor final : Janitor
    {
        std::size_t cleanupCalls = 0;
        std::vector<void*> cleanedPtrs;

        void cleanup(void* ptr) override
        {
            ++cleanupCalls;
            cleanedPtrs.push_back(ptr);
        }
    };

    template <SizeType MaxThreads = 2, SizeType MaxRetired = 16>
    using TestDomain = EpochBasedDomain<EpochBasedDomainTraits<MaxThreads, MaxRetired>>;

    TEST(EpochBasedReclamationTests, StartsAtEpochOne)
    {
        TestDomain<> domain;

        EXPECT_EQ(domain.getCurrentEpoch(), 1u);
    }

    TEST(EpochBasedReclamationTests, AdvancesEpochWhenNoThreadsAreActive)
    {
        TestDomain<> domain;

        domain.tryAdvanceEpoch();
        EXPECT_EQ(domain.getCurrentEpoch(), 2u);

        domain.tryAdvanceEpoch();
        EXPECT_EQ(domain.getCurrentEpoch(), 3u);
    }

    TEST(EpochBasedReclamationTests, GuardEntersCriticalSectionAndDestructorLeavesIt)
    {
        TestDomain<> domain;

        {
            auto guard = domain.makeGuard(/*threadIndex=*/0);

            // While the guard is alive, thread 0 is active in the current epoch,
            // so advancement must be blocked.
            domain.tryAdvanceEpoch();
            EXPECT_EQ(domain.getCurrentEpoch(), 1u);
        }

        // After guard destruction, the thread should have left the critical section.
        domain.tryAdvanceEpoch();
        EXPECT_EQ(domain.getCurrentEpoch(), 2u);
    }

    TEST(EpochBasedReclamationTests, MovedGuardStillLeavesOriginalCriticalSectionOnce)
    {
        TestDomain<> domain;

        {
            auto g1 = domain.makeGuard(/*threadIndex=*/0);

            domain.tryAdvanceEpoch();
            EXPECT_EQ(domain.getCurrentEpoch(), 1u);

            auto g2 = std::move(g1);

            // Still blocked while moved-to guard is alive.
            domain.tryAdvanceEpoch();
            EXPECT_EQ(domain.getCurrentEpoch(), 1u);

            (void)g2;
        }

        // Once the moved-to guard is destroyed, the thread should be inactive.
        domain.tryAdvanceEpoch();
        EXPECT_EQ(domain.getCurrentEpoch(), 2u);
    }

    TEST(EpochBasedReclamationTests, GuardProtectAtomicPointerReturnsCurrentPointer)
    {
        TestDomain<> domain;

        int value = 123;
        std::atomic<int*> ptr{&value};

        auto guard = domain.makeGuard(/*threadIndex=*/0);
        int* protectedPtr = guard.protectAtomicPointer(ptr);

        EXPECT_EQ(protectedPtr, &value);
    }

    TEST(EpochBasedReclamationTests, GuardResetClearsPointerSlotButCriticalSectionEndsOnDestructor)
    {
        TestDomain<> domain;

        int value = 7;

        {
            auto guard = domain.makeGuard(/*threadIndex=*/0);
            guard.protectPointer(&value);
            guard.reset();

            // reset() clears the slot and nulls the domain pointer in Guard.
            // With your updated Guard, destructor should still correctly leave
            // the critical section only if your implementation preserved that path.
            //
            // This test documents the intended behavior:
            // after scope exit, the thread must no longer block epoch advance.
        }

        domain.tryAdvanceEpoch();
        EXPECT_EQ(domain.getCurrentEpoch(), 2u);
    }

    TEST(EpochBasedReclamationTests, RetiredNodeIsNotReclaimedBeforeTwoEpochGracePeriod)
    {
        TestDomain<> domain;
        CountingJanitor janitor;

        int value = 42;

        domain.retire(/*threadId=*/0, &value, &janitor);

        domain.collect(0);
        EXPECT_EQ(janitor.cleanupCalls, 0u);

        domain.tryAdvanceEpoch(); // 1 -> 2
        domain.collect(0);
        EXPECT_EQ(domain.getCurrentEpoch(), 2u);
        EXPECT_EQ(janitor.cleanupCalls, 0u);

        domain.tryAdvanceEpoch(); // 2 -> 3
        domain.collect(0);
        EXPECT_EQ(domain.getCurrentEpoch(), 3u);
        EXPECT_EQ(janitor.cleanupCalls, 1u);

        ASSERT_EQ(janitor.cleanedPtrs.size(), 1u);
        EXPECT_EQ(janitor.cleanedPtrs[0], &value);
    }

    TEST(EpochBasedReclamationTests, MultipleRetiredNodesAreReclaimedOnceTheyBecomeSafe)
    {
        TestDomain<> domain;
        CountingJanitor janitor;

        int a = 1;
        int b = 2;
        int c = 3;

        domain.retire(0, &a, &janitor);
        domain.retire(0, &b, &janitor);
        domain.retire(0, &c, &janitor);

        domain.collect(0);
        EXPECT_EQ(janitor.cleanupCalls, 0u);

        domain.tryAdvanceEpoch(); // 1 -> 2
        domain.collect(0);
        EXPECT_EQ(janitor.cleanupCalls, 0u);

        domain.tryAdvanceEpoch(); // 2 -> 3
        domain.collect(0);

        EXPECT_EQ(janitor.cleanupCalls, 3u);
        ASSERT_EQ(janitor.cleanedPtrs.size(), 3u);
        EXPECT_EQ(janitor.cleanedPtrs[0], &a);
        EXPECT_EQ(janitor.cleanedPtrs[1], &b);
        EXPECT_EQ(janitor.cleanedPtrs[2], &c);
    }

    TEST(EpochBasedReclamationTests, ActiveGuardPreventsReclamationProgressUntilReleased)
    {
        TestDomain<> domain;
        CountingJanitor janitor;

        int value = 99;

        {
            auto guard = domain.makeGuard(/*threadIndex=*/0);

            domain.retire(/*threadId=*/1, &value, &janitor);

            // Thread 0 is active in epoch 1, so advancement should be blocked.
            domain.tryAdvanceEpoch();
            EXPECT_EQ(domain.getCurrentEpoch(), 1u);

            domain.collect(1);
            EXPECT_EQ(janitor.cleanupCalls, 0u);
        }

        // After guard destruction, advancement can proceed.
        domain.tryAdvanceEpoch(); // 1 -> 2
        domain.collect(1);
        EXPECT_EQ(janitor.cleanupCalls, 0u);

        domain.tryAdvanceEpoch(); // 2 -> 3
        domain.collect(1);
        EXPECT_EQ(janitor.cleanupCalls, 1u);
        EXPECT_EQ(janitor.cleanedPtrs[0], &value);
    }

    TEST(EpochBasedReclamationTests, FullRetireListGetsFreedByCollectDuringRetireWhenOldNodesBecomeSafe)
    {
        TestDomain<2, 2> domain;
        CountingJanitor janitor;

        int a = 1;
        int b = 2;
        int c = 3;

        domain.retire(0, &a, &janitor);
        domain.retire(0, &b, &janitor);
        EXPECT_EQ(janitor.cleanupCalls, 0u);

        domain.tryAdvanceEpoch(); // 1 -> 2
        domain.tryAdvanceEpoch(); // 2 -> 3

        // retire() should first collect because the retire list is full.
        domain.retire(0, &c, &janitor);

        EXPECT_EQ(janitor.cleanupCalls, 2u);
        ASSERT_EQ(janitor.cleanedPtrs.size(), 2u);
        EXPECT_EQ(janitor.cleanedPtrs[0], &a);
        EXPECT_EQ(janitor.cleanedPtrs[1], &b);

        domain.collect(0);
        EXPECT_EQ(janitor.cleanupCalls, 2u);

        domain.tryAdvanceEpoch(); // 3 -> 4
        domain.collect(0);
        EXPECT_EQ(janitor.cleanupCalls, 2u);

        domain.tryAdvanceEpoch(); // 4 -> 5
        domain.collect(0);
        EXPECT_EQ(janitor.cleanupCalls, 3u);
        EXPECT_EQ(janitor.cleanedPtrs[2], &c);
    }
}
