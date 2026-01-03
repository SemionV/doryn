#pragma once
#include <array>
#include <atomic>
#include <dory/macros/assert.h>

namespace dory::data_structures::containers::lockfree
{
    /*Vector-like lock-free data structure*/
    template<typename T, typename TAllocator, std::size_t SEGMENT_SIZE = 64, std::size_t MAX_SEGMENTS = 256>
    class SegmentedList
    {
    public:
        using size_type = std::size_t;

    private:
        TAllocator& _allocator;
        std::atomic<T*> _segments[MAX_SEGMENTS];
        std::atomic<size_type> _size = 0;

    public:
        explicit SegmentedList(TAllocator& allocator):
            _allocator(allocator)
        {}

    private:
        void allocateSegment(const size_type index)
        {
            assert::inhouse(index < MAX_SEGMENTS, "Segment index out of range");

            constexpr size_type bytes = sizeof(T) * SEGMENT_SIZE;

            // optional fast-path
            if (_segments[index].load(std::memory_order_acquire) != nullptr)
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
    };
}
