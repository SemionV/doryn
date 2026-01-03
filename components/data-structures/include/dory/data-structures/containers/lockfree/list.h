#pragma once
#include <array>
#include <atomic>
#include <dory/macros/assert.h>
#include <dory/generic/concepts.h>

namespace dory::data_structures::containers::lockfree
{
    /*Vector-like lock-free data structure*/
    template<typename T, typename TAllocator, std::size_t SEGMENT_SIZE = 64, std::size_t MAX_SEGMENTS = 256>
    requires(generic::concepts::is_power_of_two<SEGMENT_SIZE> && generic::concepts::is_power_of_two<MAX_SEGMENTS>) // both must be power of two
    class SegmentedList
    {
    public:
        using size_type = std::size_t;

    private:
        static constexpr size_type SEGMENT_SHIFT = std::countr_zero(SEGMENT_SIZE);

        TAllocator& _allocator;
        std::atomic<T*> _segments[MAX_SEGMENTS];
        std::atomic<size_type> _size = 0;

    public:
        explicit SegmentedList(TAllocator& allocator):
            _allocator(allocator)
        {
            for(size_type i = 0; i < MAX_SEGMENTS; ++i)
            {
                _segments[i].store(nullptr, std::memory_order_relaxed);
            }
        }

        [[nodiscard]] size_type size() const
        {
            return _size.load(std::memory_order::relaxed);
        }

        size_type append()
        {
            // Reserve an index (ticket dispenser)
            const size_type index = _size.fetch_add(1, std::memory_order_relaxed);

            const size_type segmentIndex = getSegmentIndex(index);
            allocateSegment(segmentIndex);

            return index;
        }

    private:
        void allocateSegment(const size_type index)
        {
            assert::inhouse(index < MAX_SEGMENTS, "Segment index out of range");

            constexpr size_type bytes = sizeof(T) * SEGMENT_SIZE;

            // optional fast-path
            if (_segments[index].load(std::memory_order::acquire) != nullptr)
                return;

            T* segment = static_cast<T*>(_allocator.allocate(bytes));
            assert::inhouse(segment, "Cannot allocate memory segment for list");
            if (!segment) return;

            T* expected = nullptr;
            if (!_segments[index].compare_exchange_strong(
                    expected, segment,
                    std::memory_order_release,
                    std::memory_order_relaxed))
            {
                _allocator.deallocate(segment, bytes);
            }
        }

        static size_type getSegmentIndex(const size_type globalIndex)
        {
            return globalIndex >> SEGMENT_SHIFT;
        }

        static size_type getOffset(const size_type globalIndex)
        {
            return globalIndex & (SEGMENT_SIZE - 1); //fast modulo operation on power-of-two numbers
        }
    };
}
