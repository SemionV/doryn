#pragma once

#include <cstddef>

#include "iBlockAllocProfiler.h"

namespace dory::memory::profilers
{
    class BlockAuditProfiler: public IBlockAllocProfiler
    {
    public:
        struct BlockAllocAudit
        {
            std::size_t allocatedSize {};
            std::size_t freedSize {};
            std::size_t failedSize {};
        };

    private:
        BlockAllocAudit _blockAllocAudit;

    public:
        void traceBlockAlloc(void* ptr, std::size_t size) final;
        void traceBlockFree(void* ptr, std::size_t size) final;
        void traceBlockFree(void* ptr) final;
        void traceBlocAllocFailure(std::size_t size) final;
    };
}