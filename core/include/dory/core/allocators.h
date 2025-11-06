#pragma once

#include <dory/memory/allocators/segregationAllocator.h>
#include "dory/memory/allocators/standardAllocator.h"
#include "dory/memory/allocators/systemAllocator.h"
#include "allocationProfiler.h"

namespace dory::core
{
    using GlobalAllocatorType = memory::SegregationAllocator<10, memory::PageAllocator, memory::SystemAllocator, memory::SystemAllocator, AllocProfiler>;
}