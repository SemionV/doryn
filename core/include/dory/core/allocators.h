#pragma once

#include <dory/memory/allocators/general/segregationAllocator.h>
#include "dory/memory/allocators/general/systemAllocator.h"
#include "dory/memory/allocators/specific/pageBlockAllocator.h"

namespace dory::core
{
    using GlobalAllocatorType = memory::allocators::general::SegregationAllocator<10, memory::allocators::specific::PageBlockAllocator, memory::allocators::general::SystemAllocator, memory::allocators::general::SystemAllocator>;
}
