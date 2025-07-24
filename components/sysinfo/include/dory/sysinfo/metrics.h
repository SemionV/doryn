#pragma once
#include <cstdint>

namespace dory::sysinfo
{
    struct ProcessMemoryState
    {
        std::size_t virtualMemorySize {}; //Total virtual address space used
        std::size_t residentSetSize {}; //Amount of physical memory currently used
        std::size_t memoryMapCount {}; //Number of mmap'd regions
        std::size_t sharedMemoryUsage {}; //Shared with other processes (e.g., libraries)
        std::size_t privateMemoryUsage {}; //Memory only accessible to the process
        std::size_t memoryFragmentation {}; //Degree of fragmentation in heap
        std::size_t workingSetSize {}; //Set of recently used memory pages
        std::size_t dirtyPages {}; //Pages modified but not yet flushed to disk
        std::size_t dataSize {}; //Heap memory + Global/static variables that are writable + Memory mapped regions created via mmap with MAP_PRIVATE and write access
        std::size_t stackSize {}; //The amount of virtual memory reserved for the process stack
        std::size_t pageIORate {}; //Rate of memory being paged in/out
        std::size_t memoryBandwidth {}; //Bytes/sec being read/written to RAM
        std::size_t numaLocality {}; //Memory accesses from non-local nodes
    };

    struct MemoryEventCounters
    {
        std::size_t minorPageFaults {}; //Page faults when pages were not commited yet
        std::size_t majorPageFaults {}; //Page faults when pages were loaded from hard drive
        std::size_t memoryAllocations {}; //Heap Memory allocations(malloc)
        std::size_t memoryDeallocations {}; //Heap Memory deallocations(free)
        std::size_t tlbMisses {}; //Misses in Translation Lookaside Buffer (MMU cache)
        std::size_t pageSwapCount {}; //Pages swapped from/to disk
    };

    struct ProcessMetrics
    {
        std::uint16_t threadCount {};
        ProcessMemoryState memoryState;
        MemoryEventCounters memoryEvents;
    };
}
