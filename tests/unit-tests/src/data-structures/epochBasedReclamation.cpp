#include <gtest/gtest.h>
#include <atomic>
#include <optional>
#include <utility>

#include <dory/types.h>
#include <dory/data-structures/memory-reclamation/epochBasedReclamation.h>
#include <dory/data-structures/memory-reclamation/retireList.h>
#include <dory/memory/allocators/general/systemAllocator.h>

namespace dory::data_structures::memory_reclamation::ebr::tests
{
    template <typename T, typename TDomain, typename TAllocator, LabelType AllocLabel = {}>
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
        TDomain& _domain;
        std::atomic<Node*> _head{nullptr};
        TAllocator& _allocator;
        memory_reclamation::ObjectJanitor<typename TDomain::RetiredNodeType, TAllocator> _janitor;

    public:
        explicit TreiberStack(TDomain& domain, TAllocator& allocator):
            _domain(domain),
            _allocator(allocator),
            _janitor(allocator)
        {}

        TreiberStack(const TreiberStack&) = delete;
        TreiberStack& operator=(const TreiberStack&) = delete;

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
            auto guard = _domain.pin(thread_index);

            Node* old_head = _head.load(std::memory_order_acquire);

            while (old_head)
            {
                Node* next = old_head->next;
                if (_head.compare_exchange_weak(
                        old_head,
                        next,
                        std::memory_order_acq_rel,
                        std::memory_order_acquire))
                {
                    std::optional<T> result{std::move(old_head->value)};

                    // Guard still active here, which is fine.
                    // Retirement does not free immediately.
                    _domain.retire(thread_index, old_head, &_janitor);

                    return result;
                }
            }

            return std::nullopt;
        }

        bool empty() const noexcept
        {
            return _head.load(std::memory_order_acquire) == nullptr;
        }
    };

    void test()
    {
        using Domain = Domain<8, 128>;
        Domain domain;

        using AllocatorType = memory::allocators::general::SystemAllocator;

        AllocatorType allocator;
        TreiberStack<int, Domain, AllocatorType> stack(domain, allocator);

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
                        domain.collectAll();
                }
            });
        }

        for (auto& t : workers)
            t.join();

        domain.collectAll();
        domain.collectAll();
        domain.collectAll();

        std::cout << "Done\n";
    }

    TEST(EpochBasedReclamationTests, TreiberStack)
    {
        test();
    }
}
