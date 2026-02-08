#pragma once
#include <iostream>
#include <spdlog/fmt/fmt.h>

#include "dory/memory/allocators/segregationAllocator.h"
#include "dory/memory/allocators/standardAllocator.h"
#include "dory/memory/allocators/systemAllocator.h"

#include <dory/memory/profilers/memClassAuditProfiler.h>

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

    class AllocProfiler
    {
    public:
        void traceSlotAlloc(void* ptr, std::size_t size, std::size_t slotSize, std::size_t classIndex)
        {
            std::cout << fmt::format("Slot allocated: size [{0}], slot[{1}], class[{2}], ptr[{3}]", size, slotSize, classIndex, ptr) << std::endl;
        }

        void traceSlotFree(void* ptr, std::size_t slotSize, std::size_t classIndex)
        {
            std::cout << fmt::format("Slot deallocated: slot[{0}], class[{1}], ptr[{2}]", slotSize, classIndex, ptr) << std::endl;
        }

        void traceLargeAlloc(void* ptr, std::size_t size)
        {
            std::cout << fmt::format("Large object allocated: size [{0}], ptr[{1}]", size, ptr) << std::endl;
        }

        void traceLargeFree(void* ptr, std::size_t size)
        {
            std::cout << fmt::format("Large object deallocated: ptr[{0}], size[{1}]", ptr, size) << std::endl;
        }
    };

    constexpr static std::size_t MEMORY_CLASS_COUNT = 11;

    template<typename TAllocatorProfiler = memory::profilers::MemoryClassAuditProfiler<MEMORY_CLASS_COUNT>>
    class AllocatorBuilder
    {
    private:
        constexpr static std::size_t PAGE_SIZE = 4096;

        memory::PageAllocator _blockAllocator { PAGE_SIZE };
        memory::SystemAllocator _systemAllocator;
        std::array<memory::MemorySizeClass, MEMORY_CLASS_COUNT> _sizeClasses = {
            memory::MemorySizeClass{ 8, 1024 },
            memory::MemorySizeClass{ 16, 1024 },
            memory::MemorySizeClass{ 32, 1024 },
            memory::MemorySizeClass{ 64, 1024 },
            memory::MemorySizeClass{ 128, 1024 },
            memory::MemorySizeClass{ 256, 1024 },
            memory::MemorySizeClass{ 512, 1024 },
            memory::MemorySizeClass{ 1024, 1024 },
            memory::MemorySizeClass{ 2048, 1024 },
            memory::MemorySizeClass{ 4096, 1024 },
            memory::MemorySizeClass{ 8192, 512 }
        };
        TAllocatorProfiler _profiler {_sizeClasses};

    public:
        using SegregationAllocatorType = memory::SegregationAllocator<MEMORY_CLASS_COUNT, memory::PageAllocator, memory::SystemAllocator, memory::SystemAllocator, TAllocatorProfiler>;

        template<typename T>
        using StandardAllocatorType = memory::StandardAllocator<T, SegregationAllocatorType>;

        std::shared_ptr<SegregationAllocatorType> build(const char* name = _defaultName.data())
        {
            return std::make_shared<SegregationAllocatorType>(name, _blockAllocator, _systemAllocator, _systemAllocator, _profiler, _sizeClasses);
        }

    private:
        static constexpr std::string_view _defaultName = "testSegAlloc";
    };
}
