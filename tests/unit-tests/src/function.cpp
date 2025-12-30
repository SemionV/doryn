#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/fmt/fmt.h>
#include <dory/data-structures/function.h>
#include <dory/memory/allocators/segregationAllocator.h>
#include <dory/memory/allocators/standardAllocator.h>
#include <dory/memory/allocators/systemAllocator.h>

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

    void traceLargeFree(void* ptr)
    {
        std::cout << fmt::format("Large object deallocated: ptr[{0}]", ptr) << std::endl;
    }
};

using SegregationAllocatorType = dory::memory::SegregationAllocator<10, dory::memory::PageAllocator, dory::memory::SystemAllocator, dory::memory::SystemAllocator, AllocProfiler>;

template<typename T>
using StandardAllocatorType = dory::memory::StandardAllocator<T, SegregationAllocatorType>;

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

    [[nodiscard]] bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
        return this == &other;
    }
};

std::shared_ptr<SegregationAllocatorType> buildAllocatorFunction()
{
    constexpr std::size_t PAGE_SIZE = 4096;
    dory::memory::PageAllocator blockAllocator {PAGE_SIZE};
    dory::memory::SystemAllocator systemAllocator;

    std::array sizeClasses {
        dory::memory::MemorySizeClass{ 8, 1024 },
        dory::memory::MemorySizeClass{ 16, 1024 },
        dory::memory::MemorySizeClass{ 32, 1024 },
        dory::memory::MemorySizeClass{ 64, 1024 },
        dory::memory::MemorySizeClass{ 128, 1024 },
        dory::memory::MemorySizeClass{ 256, 1024 },
        dory::memory::MemorySizeClass{ 512, 1024 },
        dory::memory::MemorySizeClass{ 1024, 1024 },
        dory::memory::MemorySizeClass{ 2048, 1024 },
        dory::memory::MemorySizeClass{ 4096, 1024 }
    };

    AllocProfiler profiler;

    return std::make_shared<SegregationAllocatorType>("testSegAlloc", blockAllocator, systemAllocator, systemAllocator, profiler, sizeClasses);
}

TEST(FunctionWrapperTests, wrapDelegate)
{
    const auto allocator = buildAllocatorFunction();
    SegregationResource globalResource{ *allocator };

    int externalVariable = 1;

    auto delegate = [&externalVariable](const int param)
    {
        std::cout << "Delegate param: " << param << std::endl;
        std::cout << "Delegate external variable: " << externalVariable << std::endl;
    };

    const auto function = dory::data_structures::function::UniqueFunction<void(int)>{ &globalResource, delegate };
    function(2);
}

class Handler
{
private:
    int _classVariable = 1;

public:
    void foo(const int param) const
    {
        std::cout << "Handler::foo param: " << param << std::endl;
        std::cout << "Handler::foo internal variable: " << _classVariable << std::endl;
    }
};

TEST(FunctionWrapperTests, wrapClassMember)
{
    const auto allocator = buildAllocatorFunction();
    SegregationResource globalResource{ *allocator };

    Handler h;
    const dory::data_structures::function::UniqueFunction<void(int)> function { &globalResource, dory::data_structures::function::bindMember(&h, &Handler::foo) };
    function(2);
}

void freeFunction(const int param)
{
    std::cout << "freeFunction param: " << param << std::endl;
}

TEST(FunctionWrapperTests, wrapFreeFunction)
{
    const auto allocator = buildAllocatorFunction();
    SegregationResource globalResource{ *allocator };

    const auto function = dory::data_structures::function::UniqueFunction<void(int)>{ &globalResource, &freeFunction };
    function(2);
}