#include <gtest/gtest.h>
#include <atomic>
#include <optional>
#include <utility>

#include <dory/data-structures/memory-reclamation/epochBasedReclamation.h>

namespace dory::data_structures::memory_reclamation::ebr::tests
{
    template <typename T, typename TDomain>
    class TreiberStack
    {
        struct Node
        {
            T value;
            Node* next = nullptr;

            template <typename U>
            explicit Node(U&& v) : value(std::forward<U>(v)) {}
        };

    private:
        TDomain& m_domain;
        std::atomic<Node*> m_head{nullptr};

    public:
        explicit TreiberStack(TDomain& domain) noexcept
            : m_domain(domain)
        {
        }

        TreiberStack(const TreiberStack&) = delete;
        TreiberStack& operator=(const TreiberStack&) = delete;

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
            auto guard = m_domain.pin(thread_index);

            Node* old_head = m_head.load(std::memory_order_acquire);

            while (old_head)
            {
                Node* next = old_head->next;
                if (m_head.compare_exchange_weak(
                        old_head,
                        next,
                        std::memory_order_acq_rel,
                        std::memory_order_acquire))
                {
                    std::optional<T> result{std::move(old_head->value)};

                    // Guard still active here, which is fine.
                    // Retirement does not free immediately.
                    m_domain.retire(thread_index, old_head);

                    return result;
                }
            }

            return std::nullopt;
        }

        bool empty() const noexcept
        {
            return m_head.load(std::memory_order_acquire) == nullptr;
        }
    };

    void test()
    {
        using Domain = Domain<8, 128>;
        Domain domain;

        TreiberStack<int, Domain> stack(domain);

        for (int i = 0; i < 1000; ++i)
            stack.push(i);

        std::vector<std::thread> workers;
        for (std::uint32_t tid = 0; tid < 4; ++tid)
        {
            workers.emplace_back([tid, &stack, &domain]()
            {
                for (;;)
                {
                    auto v = stack.pop(tid);
                    if (!v.has_value())
                        break;

                    // Do work...

                    // Periodic maintenance.
                    if (((*v) & 31) == 0)
                        domain.collect_all();
                }
            });
        }

        for (auto& t : workers)
            t.join();

        domain.collect_all();
        domain.collect_all();
        domain.collect_all();

        std::cout << "Done\n";
    }

    TEST(EpochBasedReclamationTests, TreiberStack)
    {
        test();
    }
}
