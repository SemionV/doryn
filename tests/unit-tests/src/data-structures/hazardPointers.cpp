#include <dory/data-structures/memory-reclamation/hazardPointers.h>

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <iostream>

namespace dory::data_structures::memory_reclamation::hazard_pointers::tests
{
    template <typename T, typename DomainT>
    class HazardTreiberStack
    {
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

    public:
        explicit HazardTreiberStack(DomainT& domain) noexcept
            : m_domain(domain)
        {
        }

        HazardTreiberStack(const HazardTreiberStack&) = delete;
        HazardTreiberStack& operator=(const HazardTreiberStack&) = delete;

        template <typename U>
        void push(U&& value)
        {
            Node* node = new Node(std::forward<U>(value));

            Node* old_head = m_head.load(std::memory_order_relaxed);
            do
            {
                node->next = old_head;
            }
            while (!m_head.compare_exchange_weak(
                old_head,
                node,
                std::memory_order_release,
                std::memory_order_relaxed));
        }

        std::optional<T> pop(u32 thread_index)
        {
            auto guard = m_domain.make_guard(thread_index, 0);

            for (;;)
            {
                Node* head = guard.get_protected(m_head);
                if (!head)
                    return std::nullopt;

                Node* next = head->next;

                if (m_head.compare_exchange_weak(
                        head,
                        next,
                        std::memory_order_acq_rel,
                        std::memory_order_acquire))
                {
                    std::optional<T> result{std::move(head->value)};

                    // Clear hazard before retirement scan opportunities elsewhere.
                    guard.reset();

                    m_domain.retire(thread_index, head);
                    return result;
                }

                // CAS failed; loop and re-protect the new head.
            }
        }

        bool empty() const noexcept
        {
            return m_head.load(std::memory_order_acquire) == nullptr;
        }

    private:
        DomainT& m_domain;
        std::atomic<Node*> m_head{nullptr};
    };

    void test()
    {
        using Domain = Domain<8, 2, 128>;
        Domain domain;

        HazardTreiberStack<int, Domain> stack(domain);

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