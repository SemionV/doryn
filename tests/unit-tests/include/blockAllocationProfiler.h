#pragma once

#include <dory/memory/profilers/iBlockAllocationProfiler.h>

#include "gtest/gtest.h"

namespace dory::test_utilities
{
    class BlockAllocationProfiler : public memory::profilers::IBlockAllocationProfiler
    {
    public:
        struct BlockFree
        {
            void* ptr;
            std::size_t size;
            std::size_t alignment;
        };

        struct BlockAllocation: BlockFree
        {
            LabelType label;
        };

        std::vector<BlockAllocation> blockAllocations;
        std::vector<BlockFree> blockFrees;

    public:
        void traceBlockAllocation(void* ptr, std::size_t size, std::size_t alignment, LabelType label) final;
        void traceBlockFree(void* ptr, std::size_t size, std::size_t alignment) final;
    };
}
