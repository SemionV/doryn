#pragma once

#include <cstddef>
#include <dory/types.h>
#include <dory/base.h>
#include <dory/memory/profilers/iBlockAllocationProfiler.h>

namespace dory::memory::allocators::specific
{
    template<typename TImplementation>
    class BlockAllocator: public Base<TImplementation>
    {
    private:
        profilers::IBlockAllocationProfiler* _profiler;

    public:
        explicit BlockAllocator(profilers::IBlockAllocationProfiler* profiler):
            _profiler(profiler)
        {}

        void* allocateBlock(LabelType label, std::size_t multiple)
        {
            void* ptr = this->implRef().allocateBlockImpl(label, multiple);

            if(ptr && _profiler)
            {
                const std::size_t blockSize = getBlockSize();
                _profiler->traceBlockAllocation(ptr, blockSize * multiple, blockSize, label);
            }

            return ptr;
        }

        void deallocateBlock(void* ptr, std::size_t multiple)
        {
            this->implRef().deallocateBlockImpl(ptr, multiple);

            if(ptr && _profiler)
            {
                const std::size_t blockSize = getBlockSize();
                _profiler->traceBlockFree(ptr, blockSize * multiple, blockSize);
            }
        }

        [[nodiscard]] std::size_t getBlockSize()
        {
            return this->implRef().getBlockSizeImpl();
        }
    };
}
