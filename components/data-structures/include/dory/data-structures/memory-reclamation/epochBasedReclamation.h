#pragma once

#include <array>
#include <atomic>

#include <dory/types.h>
#include <dory/constants.h>
#include <dory/macros/assert.h>

#include "janitor.h"
#include "retireList.h"
#include "guard.h"
#include "domain.h"

namespace dory::data_structures::memory_reclamation::ebr
{
    struct EpochRetiredNode: public RetiredNode
    {
        u64 retireEpoch = 0;
    };

    struct alignas(constants::CacheLineSize) ThreadEpochState
    {
        std::atomic<u64> localEpoch{0};
        std::atomic<bool> active{false};
    };

    template <typename TDomainTraits>
    class EpochBasedDomain;

    template<SizeType MaxThreads, SizeType MaxRetiredPerThread>
    struct EpochBasedDomainTraits
    {
        using RetiredNodeType = EpochRetiredNode;
        using RetireListType = RetireList<RetiredNodeType, MaxRetiredPerThread>;
        using GuardType = Guard<EpochBasedDomain<EpochBasedDomainTraits>>;
        static constexpr SizeType maxThreads = MaxThreads;
        static constexpr SizeType maxRetiredPerThread = MaxRetiredPerThread;
        static constexpr SizeType pointerSlotsPerThread = std::numeric_limits<SizeType>::max();
    };

    template <typename TDomainTraits>
    class EpochBasedDomain: public MemoryReclamationDomain<EpochBasedDomain<TDomainTraits>, TDomainTraits>
    {
        friend class MemoryReclamationDomain<EpochBasedDomain, TDomainTraits>;

    private:
        alignas(constants::CacheLineSize) std::atomic<u64> _globalEpoch {1};
        std::array<ThreadEpochState, TDomainTraits::maxThreads> _threads {};
        std::atomic_flag _collecting = ATOMIC_FLAG_INIT;

    public:
        static SizeType getPointerSlotImpl(ThreadId threadId, PointerToken pointerToken)
        {
            return {};
        }

        void enterImpl(ThreadId threadId)
        {
            ThreadEpochState& ts = _threads[threadId];

            // Publish the current epoch first, then mark active.
            const u64 epoch = _globalEpoch.load(std::memory_order_acquire);
            ts.localEpoch.store(epoch, std::memory_order_relaxed);
            ts.active.store(true, std::memory_order_release);

            // Re-read global epoch in case it advanced concurrently.
            const u64 verify = _globalEpoch.load(std::memory_order_acquire);
            if (verify != epoch)
            {
                ts.localEpoch.store(verify, std::memory_order_relaxed);
            }
        }

        void leaveImpl(ThreadId threadId)
        {
            _threads[threadId].active.store(false, std::memory_order_release);
        }

        static void clearPointerSlotImpl(const SizeType pointerSlot)
        {}

        static void occupyPointerSlotImpl(void* ptr, const SizeType pointerSlot)
        {}

        template <typename T>
        static T* occupyAtomicPointerSlotImpl(std::atomic<T*>& src, const SizeType pointerSlot)
        {
            return src.load(std::memory_order_acquire);
        }

        void tryAdvanceEpochImpl()
        {
            const u64 current = _globalEpoch.load(std::memory_order_acquire);

            for (SizeType i = 0; i < TDomainTraits::maxThreads; ++i)
            {
                const bool active = _threads[i].active.load(std::memory_order_acquire);
                if (!active)
                    continue;

                const u64 local = _threads[i].localEpoch.load(std::memory_order_acquire);

                // Active thread still in current or older epoch: cannot advance.
                if (local <= current)
                    return;
            }

            _globalEpoch.compare_exchange_strong(
                const_cast<u64&>(current),
                current + 1,
                std::memory_order_acq_rel,
                std::memory_order_acquire);
        }

        void retireImpl(ThreadId threadId, void* ptr, Janitor* janitor) noexcept
        {
            const u64 epoch = _globalEpoch.load(std::memory_order_acquire);
            auto& retireList = this->_retired[threadId];

            if (retireList.full())
            {
                this->collect(threadId);
                if (retireList.full())
                {
                    // At this point reclamation is blocked by lagging threads.
                    // In an engine you might flush later, grow capacity,
                    // or hand this to a fallback allocator queue.
                    assert::debug(false, "EBR retire list full; reclamation stalled");
                    return;
                }
            }

            retireList.push(typename TDomainTraits::RetiredNodeType { ptr, janitor, epoch });

            // Opportunistic maintenance.
            if ((retireList.count & 7u) == 0u)
            {
                this->tryAdvanceEpoch();
                this->collect(threadId);
            }
        }

        void collectImpl(ThreadId threadId) noexcept
        {
            const u64 global = _globalEpoch.load(std::memory_order_acquire);

            this->_retired[threadId].reclaimIf(
                [global](const typename TDomainTraits::RetiredNodeType& n) noexcept
                {
                    //Use module uint substruction to be safe in case of wrap-around(when globalEpoch reaches the max value possible)
                    const u64 age = global - n.retireEpoch;
                    return age >= 2;
                });
        }

        [[nodiscard]] u64 getCurrentEpoch() const noexcept
        {
            return _globalEpoch.load(std::memory_order_acquire);
        }
    };
}
