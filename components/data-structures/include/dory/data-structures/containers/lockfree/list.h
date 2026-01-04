#pragma once
#include <array>
#include <atomic>
#include <dory/macros/assert.h>
#include <dory/generic/concepts.h>

namespace dory::data_structures::containers::lockfree
{
    /*
     * Vector-like lock-free data structure
     * It consists of memory blocks(segments) and can only grow(append method)
     * It is intended to be used as base for such data structures like a generic free-list
     */
    template<typename T, typename TAllocator, std::size_t SEGMENT_SIZE = 64, std::size_t MAX_SEGMENTS = 256>
    requires(generic::concepts::is_power_of_two<SEGMENT_SIZE> && generic::concepts::is_power_of_two<MAX_SEGMENTS>) // both must be power of two
    class SegmentedList
    {
    public:
        using size_type = std::size_t;

    private:
        static constexpr size_type SEGMENT_SHIFT = std::countr_zero(SEGMENT_SIZE);
        static constexpr size_type CAPACITY = MAX_SEGMENTS * SEGMENT_SIZE;

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

        /*
         * Reserves a slot for an item and if needed allocates memory for the hosting segment
         * The consumer of this method supposed to initialize the slot properly(construct) after the append returns
         */
        size_type append()
        {
            // Reserve an index (ticket dispenser)
            const size_type index = _size.fetch_add(1, std::memory_order_relaxed);
            assert::inhouse(index < CAPACITY, "SegmentedList full");

            const size_type segmentIndex = getSegmentIndex(index);
            allocateSegment(segmentIndex);

            return index;
        }

        /*
         * Reserves memory for segments, which can hold "count" items.
         * It does not increase _size of the data structure, because each item has to be appended and initialized individually.
         */
        void reserve(const size_type count)
        {
            assert::inhouse(count <= CAPACITY, "reserve exceeds capacity");

            const size_type segmentsCount = (count + SEGMENT_SIZE - 1) >> SEGMENT_SHIFT;

            for(size_type i = 0; i < segmentsCount; ++i)
            {
                allocateSegment(i);
            }
        }

        T* getSlot(const size_type index)
        {
            return getSlot<false>(index);
        }

        const T* getSlot(const size_type index) const
        {
            return getSlot<true>(index);
        }

    private:
        /*
         * Allocates memory for segment. If memory is allocated already by another thread, deallocates the memory block and returns
         */
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

        /*
         * Returns pointer to an item slot, which has to be initialized still or for some low-level manipulation
         */
        template<bool IsConst>
        auto getSlot(const size_type index) -> std::conditional_t<IsConst, const T*, T*>
        {
            using pointer = std::conditional_t<IsConst, const T*, T*>;

            const size_type size = _size.load(std::memory_order::relaxed);
            assert::inhouse(index < size, "Invalid index");

            const size_type segmentIndex = getSegmentIndex(index);
            assert::inhouse(segmentIndex < MAX_SEGMENTS, "Invalid segment index");
            const size_type offset = getOffset(index);

            pointer segment = _segments[segmentIndex].load(std::memory_order::acquire);
            assert::inhouse(segment, "Uninitialized segment of list");

            return segment + offset;
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
