#include <gtest/gtest.h>
#include <vector>

#include <dory/data-structures/memory-reclamation/hazardPointers.h>
#include <allocatorBuilder.h>
#include <dory/types.h>

#include <dory/data-structures/memory-reclamation/retireList.h>

namespace dory::data_structures::memory_reclamation::hazard_pointers::tests
{
    struct TestNode
    {
        int value = 0;
    };

    class RecordingJanitor final : public Janitor
    {
    public:
        std::vector<void*> cleaned;

        void cleanup(void* ptr) noexcept override
        {
            cleaned.push_back(ptr);
        }
    };

    template <typename T>
    static bool containsPtr(const std::vector<void*>& cleaned, T* ptr)
    {
        for (void* p : cleaned)
        {
            if (p == static_cast<void*>(ptr))
                return true;
        }
        return false;
    }

    // ------------------------------------------------------------
    // Guard behavior
    // ------------------------------------------------------------

    TEST(HazardPointersTest, GuardProtectRawMarksHazardAndResetClearsIt)
    {
        Domain<2, 2, 8> domain;

        TestNode node { 42 };

        {
            auto guard = domain.makeGuard(0, 0);
            guard.protectRaw(&node);

            EXPECT_TRUE(domain.isHazard(&node));
        }

        EXPECT_FALSE(domain.isHazard(&node));
    }

    TEST(HazardPointersTest, GuardMoveTransfersOwnershipOfHazardSlot)
    {
        Domain<2, 2, 8> domain;
        TestNode node { 7 };

        {
            auto guard1 = domain.makeGuard(0, 0);
            guard1.protectRaw(&node);

            EXPECT_TRUE(domain.isHazard(&node));

            auto guard2 = std::move(guard1);

            // Still protected because guard2 owns the slot now.
            EXPECT_TRUE(domain.isHazard(&node));

            guard2.reset();
            EXPECT_FALSE(domain.isHazard(&node));
        }

        EXPECT_FALSE(domain.isHazard(&node));
    }

    TEST(HazardPointersTest, GetProtectedReturnsStablePointerAndPublishesHazard)
    {
        Domain<2, 2, 8> domain;

        TestNode node { 123 };
        std::atomic<TestNode*> src { &node };

        auto guard = domain.makeGuard(0, 1);
        TestNode* protectedPtr = guard.getProtected(src);

        EXPECT_EQ(protectedPtr, &node);
        EXPECT_TRUE(domain.isHazard(&node));

        guard.reset();
        EXPECT_FALSE(domain.isHazard(&node));
    }

    // ------------------------------------------------------------
    // Retire / scan behavior
    // ------------------------------------------------------------

    TEST(HazardPointersTest, RetireThenScanReclaimsUnprotectedNode)
    {
        Domain<2, 2, 8> domain;
        RecordingJanitor janitor;
        TestNode node { 1 };

        domain.retire(0, &node, &janitor);

        EXPECT_TRUE(janitor.cleaned.empty());

        domain.collect(0);

        ASSERT_EQ(janitor.cleaned.size(), 1u);
        EXPECT_EQ(janitor.cleaned[0], &node);
    }

    TEST(HazardPointersTest, ScanDoesNotReclaimProtectedNodeUntilHazardIsCleared)
    {
        Domain<2, 2, 8> domain;
        RecordingJanitor janitor;
        TestNode node { 5 };

        {
            auto guard = domain.makeGuard(0, 0);
            guard.protectRaw(&node);

            domain.retire(0, &node, &janitor);
            domain.collect(0);

            EXPECT_TRUE(janitor.cleaned.empty());
            EXPECT_TRUE(domain.isHazard(&node));
        }

        EXPECT_FALSE(domain.isHazard(&node));

        domain.collect(0);

        ASSERT_EQ(janitor.cleaned.size(), 1u);
        EXPECT_EQ(janitor.cleaned[0], &node);
    }

    TEST(HazardPointersTest, ScanRemovesRetiredNodeWithNullJanitorWithoutCrashing)
    {
        Domain<2, 2, 8> domain;
        TestNode node { 9 };

        domain.retire(0, &node, nullptr);

        EXPECT_NO_THROW(domain.collect(0));

        // Re-scanning should remain harmless and should not resurrect anything.
        EXPECT_NO_THROW(domain.collect(0));
    }

    TEST(HazardPointersTest, ScanAllReclaimsAcrossAllRetireLists)
    {
        Domain<3, 2, 8> domain;
        RecordingJanitor janitorA;
        RecordingJanitor janitorB;

        TestNode a { 11 };
        TestNode b { 22 };

        domain.retire(0, &a, &janitorA);
        domain.retire(2, &b, &janitorB);

        domain.collectAll();

        ASSERT_EQ(janitorA.cleaned.size(), 1u);
        ASSERT_EQ(janitorB.cleaned.size(), 1u);
        EXPECT_EQ(janitorA.cleaned[0], &a);
        EXPECT_EQ(janitorB.cleaned[0], &b);
    }

    TEST(HazardPointersTest, HazardInAnotherThreadStillPreventsReclamation)
    {
        Domain<2, 2, 8> domain;
        RecordingJanitor janitor;
        TestNode node { 88 };

        auto otherThreadGuard = domain.makeGuard(1, 1);
        otherThreadGuard.protectRaw(&node);

        domain.retire(0, &node, &janitor);
        domain.collect(0);

        EXPECT_TRUE(janitor.cleaned.empty());

        otherThreadGuard.reset();
        domain.collect(0);

        ASSERT_EQ(janitor.cleaned.size(), 1u);
        EXPECT_EQ(janitor.cleaned[0], &node);
    }

    // ------------------------------------------------------------
    // Batch / repeated retirement behavior
    // ------------------------------------------------------------

    TEST(HazardPointersTest, ScanReclaimsOnlyUnprotectedSubset)
    {
        Domain<2, 2, 16> domain;
        RecordingJanitor janitor;

        TestNode n1 { 1 };
        TestNode n2 { 2 };
        TestNode n3 { 3 };

        auto guard = domain.makeGuard(0, 0);
        guard.protectRaw(&n2);

        domain.retire(0, &n1, &janitor);
        domain.retire(0, &n2, &janitor);
        domain.retire(0, &n3, &janitor);

        domain.collect(0);

        EXPECT_EQ(janitor.cleaned.size(), 2u);
        EXPECT_TRUE(containsPtr(janitor.cleaned, &n1));
        EXPECT_TRUE(containsPtr(janitor.cleaned, &n3));
        EXPECT_FALSE(containsPtr(janitor.cleaned, &n2));

        guard.reset();
        domain.collect(0);

        EXPECT_EQ(janitor.cleaned.size(), 3u);
        EXPECT_TRUE(containsPtr(janitor.cleaned, &n2));
    }

    TEST(HazardPointersTest, OpportunisticScanOn16thRetireCanReclaimOlderEntries)
    {
        Domain<1, 2, 32> domain;
        RecordingJanitor janitor;

        TestNode nodes[16];

        for (int i = 0; i < 16; ++i)
        {
            nodes[i].value = i;
            domain.retire(0, &nodes[i], &janitor);
        }

        // On the 16th insert, retire() triggers a scan because (count & 15) == 0.
        EXPECT_EQ(janitor.cleaned.size(), 16u);

        for (int i = 0; i < 16; ++i)
        {
            EXPECT_TRUE(containsPtr(janitor.cleaned, &nodes[i]));
        }
    }

    // ------------------------------------------------------------
    // Destructor / drain behavior
    // ------------------------------------------------------------

    TEST(HazardPointersTest, DomainDestructorDrainsAndReclaimsUnprotectedNodes)
    {
        RecordingJanitor janitor;
        TestNode node { 77 };

        {
            Domain<2, 2, 8> domain;
            domain.retire(0, &node, &janitor);

            EXPECT_TRUE(janitor.cleaned.empty());
        }

        ASSERT_EQ(janitor.cleaned.size(), 1u);
        EXPECT_EQ(janitor.cleaned[0], &node);
    }

    TEST(HazardPointersTest, DrainDoesNotReclaimStillProtectedNodes)
    {
        RecordingJanitor janitor;
        TestNode node { 101 };

        {
            Domain<2, 2, 8> domain;
            auto guard = domain.makeGuard(0, 0);
            guard.protectRaw(&node);

            domain.retire(1, &node, &janitor);
            domain.drain();

            EXPECT_TRUE(janitor.cleaned.empty());

            guard.reset();
            domain.drain();
        }

        ASSERT_EQ(janitor.cleaned.size(), 1u);
        EXPECT_EQ(janitor.cleaned[0], &node);
    }
}