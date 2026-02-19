#pragma once
#include <spdlog/fmt/fmt.h>

#include "dory/memory/allocators/general/segregationAllocator.h"
#include "dory/memory/allocators/general/systemAllocator.h"
#include <dory/memory/allocators/pageAllocator.h>

#include <dory/memory/profilers/blockAuditProfiler.h>

namespace dory::test_utilities
{
    template<typename TAllocator>
    class SegregationResource final : public std::pmr::memory_resource
    {
    public:
        explicit SegregationResource(TAllocator& allocator):
            _allocator(&allocator)
        {}

    private:
        TAllocator* _allocator;

        void* do_allocate(const std::size_t bytes, std::size_t alignment) override
        {
            return _allocator->allocate(bytes);
        }

        void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
        {
            _allocator->deallocate(p, bytes);
        }

        [[nodiscard]] bool do_is_equal(const memory_resource& other) const noexcept override
        {
            return this == &other;
        }
    };

    class AllocatorBuilder
    {
    public:
        constexpr static std::size_t MEMORY_CLASS_COUNT = 11;

        using SystemAllocatorType = memory::allocators::general::SystemAllocator;
        using PageAllocatorType = memory::PageAllocator;
        using SegregationAllocatorType = memory::allocators::general::SegregationAllocator<MEMORY_CLASS_COUNT, memory::PageAllocator, SystemAllocatorType, SystemAllocatorType>;

    private:
        constexpr static std::size_t PAGE_SIZE = 4096;

        memory::PageAllocator _blockAllocator;
        memory::allocators::general::SystemAllocator _largeObjectAllocator;
        memory::allocators::general::SystemAllocator _memoryBlockNodeAllocator;

        std::array<memory::allocators::general::MemorySizeClass, MEMORY_CLASS_COUNT> _sizeClasses = {
            memory::allocators::general::MemorySizeClass{ 8, 1024 },
            memory::allocators::general::MemorySizeClass{ 16, 1024 },
            memory::allocators::general::MemorySizeClass{ 32, 1024 },
            memory::allocators::general::MemorySizeClass{ 64, 1024 },
            memory::allocators::general::MemorySizeClass{ 128, 1024 },
            memory::allocators::general::MemorySizeClass{ 256, 1024 },
            memory::allocators::general::MemorySizeClass{ 512, 1024 },
            memory::allocators::general::MemorySizeClass{ 1024, 1024 },
            memory::allocators::general::MemorySizeClass{ 2048, 1024 },
            memory::allocators::general::MemorySizeClass{ 4096, 1024 },
            memory::allocators::general::MemorySizeClass{ 8192, 512 }
        };

    public:
        explicit AllocatorBuilder(memory::profilers::IBlockAllocProfiler* blockAllocProfiler = nullptr,
            memory::profilers::IAllocatorProfiler* largeObjectAllocProfiler = nullptr,
            memory::profilers::IAllocatorProfiler* memoryBlockNodeAllocProfiler = nullptr);

        std::shared_ptr<SegregationAllocatorType> build(memory::profilers::IAllocatorProfiler* profiler = nullptr);
    };
}
