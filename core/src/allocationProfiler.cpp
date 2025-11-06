#include <dory/core/allocationProfiler.h>
#include <spdlog/fmt/fmt.h>

namespace dory::core
{
    void AllocProfiler::traceSlotAlloc(void* ptr, std::size_t size, std::size_t slotSize, std::size_t classIndex)
    {
        std::cout << fmt::format("Slot allocated: size [{0}], slot[{1}], class[{2}], ptr[{3}]", size, slotSize, classIndex, ptr) << std::endl;
    }

    void AllocProfiler::traceSlotFree(void* ptr, std::size_t slotSize, std::size_t classIndex)
    {
        std::cout << fmt::format("Slot deallocated: slot[{0}], class[{1}], ptr[{2}]", slotSize, classIndex, ptr) << std::endl;
    }

    void AllocProfiler::traceLargeAlloc(void* ptr, std::size_t size)
    {
        std::cout << fmt::format("Large object allocated: size [{0}], ptr[{1}]", size, ptr) << std::endl;
    }

    void AllocProfiler::traceLargeFree(void* ptr)
    {
        std::cout << fmt::format("Large object deallocated: ptr[{0}]", ptr) << std::endl;
    }
}