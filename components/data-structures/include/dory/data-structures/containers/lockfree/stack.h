#pragma once

#include <atomic>
#include <dory/types.h>
#include <dory/data-structures/memory-reclamation/janitor.h>
#include <dory/data-structures/memory-reclamation/hazardPointers.h>
#include <dory/data-structures/containers/lockfree/resources/node.h>

namespace dory::data_structures::containers::lockfree
{
    /// Documentation is available here: docs/components/data-structures/hazardTreiberStack.md
    template <typename T, typename TDomain, typename TAllocator, LabelType AllocLabel = {}>
    class HazardTreiberStack
    {
    private:
        using NodeType = resources::Node<T>;

        TDomain& _domain;
        std::atomic<NodeType*> _head { nullptr };
        TAllocator& _allocator;
        memory_reclamation::ObjectJanitor<NodeType, TAllocator> _janitor;

    public:
        explicit HazardTreiberStack(TDomain& domain, TAllocator& allocator) noexcept :
            _domain(domain),
            _allocator(allocator),
            _janitor(allocator)
        {}

        HazardTreiberStack(const HazardTreiberStack&) = delete;
        HazardTreiberStack& operator=(const HazardTreiberStack&) = delete;

        template <typename U>
        void push(U&& value)
        {
            NodeType* node = _allocator.template allocateObject<NodeType>(AllocLabel, std::forward<U>(value));

            NodeType* oldHead = _head.load(std::memory_order_relaxed);
            do
            {
                node->next = oldHead;
            }
            while (!_head.compare_exchange_weak(
                oldHead,
                node,
                std::memory_order_release,
                std::memory_order_relaxed));
        }

        std::optional<T> pop(ThreadId threadIndex)
        {
            auto guard = _domain.makeGuard(threadIndex, 0);

            for (;;)
            {
                NodeType* head = guard.getProtected(_head);
                if (!head)
                    return std::nullopt;

                NodeType* next = head->next;

                if (_head.compare_exchange_weak(
                        head,
                        next,
                        std::memory_order_acq_rel,
                        std::memory_order_acquire))
                {
                    std::optional<T> result { std::move(head->value) };

                    // Clear hazard before retirement scan opportunities elsewhere.
                    guard.reset();

                    _domain.retire(threadIndex, head, &_janitor);
                    return result;
                }

                // CAS failed; loop and re-protect the new head.
            }
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return _head.load(std::memory_order_acquire) == nullptr;
        }
    };
}