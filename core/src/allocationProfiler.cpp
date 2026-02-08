#include <dory/core/allocationProfiler.h>
#include <spdlog/fmt/fmt.h>

namespace dory::core
{
    void AllocProfiler::traceSlotAlloc(void* ptr, std::size_t size, std::size_t slotSize, std::size_t classIndex)
    {
        print("Slot allocated: size [{0}], slot[{1}], class[{2}], ptr[{3}]", size, slotSize, classIndex, ptr);
    }

    void AllocProfiler::traceSlotFree(void* ptr, std::size_t slotSize, std::size_t classIndex)
    {
        print("Slot deallocated: slot[{0}], class[{1}], ptr[{2}]", slotSize, classIndex, ptr);
    }

    void AllocProfiler::traceLargeAlloc(void* ptr, std::size_t size)
    {
        print("Large object allocated: size [{0}], ptr[{1}]", size, ptr);
    }

    void AllocProfiler::traceLargeFree(void* ptr, std::size_t size)
    {
        print("Large object deallocated: ptr[{0}], size[{1}]", ptr, size);
    }
}