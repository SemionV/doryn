#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <iostream>

#include <dory/data-structures/memory-reclamation/hazardPointers.h>
#include <allocatorBuilder.h>
#include <dory/types.h>

namespace dory::data_structures::memory_reclamation::hazard_pointers::tests
{
    template <typename T, typename DomainT, typename TAllocator, LabelType AllocLabel = {}>
    class HazardTreiberStack
    {
    private:
        struct Node
        {
            T value;
            Node* next = nullptr;

            template <typename U>
            explicit Node(U&& v)
                : value(std::forward<U>(v))
            {
            }
        };

        DomainT& _domain;
        std::atomic<Node*> _head { nullptr };
        TAllocator& _allocator;
        ObjectJanitor<Node, TAllocator> _janitor;

    public:
        explicit HazardTreiberStack(DomainT& domain, TAllocator& allocator) noexcept :
            _domain(domain),
            _allocator(allocator),
            _janitor(allocator)
        {}

        HazardTreiberStack(const HazardTreiberStack&) = delete;
        HazardTreiberStack& operator=(const HazardTreiberStack&) = delete;

        template <typename U>
        void push(U&& value)
        {
            Node* node = _allocator.template allocateObject<Node>(AllocLabel, std::forward<U>(value));

            Node* old_head = _head.load(std::memory_order_relaxed);
            do
            {
                node->next = old_head;
            }
            while (!_head.compare_exchange_weak(
                old_head,
                node,
                std::memory_order_release,
                std::memory_order_relaxed));
        }

        std::optional<T> pop(u32 thread_index)
        {
            auto guard = _domain.make_guard(thread_index, 0);

            for (;;)
            {
                Node* head = guard.get_protected(_head);
                if (!head)
                    return std::nullopt;

                Node* next = head->next;

                if (_head.compare_exchange_weak(
                        head,
                        next,
                        std::memory_order_acq_rel,
                        std::memory_order_acquire))
                {
                    std::optional<T> result{std::move(head->value)};

                    // Clear hazard before retirement scan opportunities elsewhere.
                    guard.reset();

                    _domain.retire(thread_index, head, &_janitor);
                    return result;
                }

                // CAS failed; loop and re-protect the new head.
            }
        }

        bool empty() const noexcept
        {
            return _head.load(std::memory_order_acquire) == nullptr;
        }
    };

    void test()
    {
        test_utilities::AllocatorBuilder allocBuilder {};
        const auto allocator = allocBuilder.build(nullptr);

        using Domain = Domain<8, 2, 128>;
        Domain domain;

        HazardTreiberStack<int, Domain, test_utilities::AllocatorBuilder::SegregationAllocatorType> stack(domain, *allocator);

        for (int i = 0; i < 1000; ++i)
            stack.push(i);

        std::vector<std::thread> workers;
        for (std::uint32_t tid = 0; tid < 4; ++tid)
        {
            workers.emplace_back([tid, &stack, &domain]()
            {
                for (;;)
                {
                    auto value = stack.pop(tid);
                    if (!value.has_value())
                        break;

                    // Simulated work...

                    if (((*value) & 31) == 0)
                        domain.scan(tid);
                }

                domain.scan(tid);
            });
        }

        for (auto& t : workers)
            t.join();

        domain.scan_all();

        std::cout << "done\n";
    }

    //--------------------------------------------------------------------------
    // Tests
    //--------------------------------------------------------------------------

    TEST(HazardPointersTests, TestUseCase)
    {
        test();
    }
}