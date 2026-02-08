#include <dory/memory/profilers/blockAuditProfiler.h>

namespace dory::memory::profilers
{
    void BlockAuditProfiler::traceBlockAlloc(void* ptr, const std::size_t size)
    {
        _blockAllocAudit.allocatedSize += size;
    }

    void BlockAuditProfiler::traceBlockFree(void* ptr, const std::size_t size)
    {
        _blockAllocAudit.freedSize += size;
    }

    void BlockAuditProfiler::traceBlockFree(void* ptr)
    {
    }

    void BlockAuditProfiler::traceBlocAllocFailure(const std::size_t size)
    {
        _blockAllocAudit.failedSize += size;
    }
}
