#pragma once

#include <dory/memory/allocators/general/segregationAllocator.h>
#include "dory/memory/allocators/standardAllocator.h"
#include "dory/memory/allocators/systemAllocator.h"
#include "allocationProfiler.h"
#include "dory/memory/allocators/specific/pageBlockAllocator.h"

namespace dory::core
{
    using GlobalAllocatorType = memory::allocators::general::SegregationAllocator<10, memory::allocators::specific::PageBlockAllocator, memory::SystemAllocator, memory::SystemAllocator, AllocProfiler>;
}
