#pragma once
#include <cstdint>
#include <chrono>

namespace dory::profiling
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

    struct ExecutionMetrics
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> wallClockStart {};
        std::chrono::time_point<std::chrono::high_resolution_clock> wallClockEnd {};
        std::size_t cpuTimeKernel {}; //CPU execution time spent in kernel space - nanoseconds
        std::size_t cpuTimeUser {}; //CPU execution time spent in user space - nanoseconds
    };

#if DORY_PLATFORM_LINUX
    //File Descriptor of perf  events
    struct EventFileDescriptors
    {
        int tlbMisses {-1};
    };
#elif DORY_PLATFORM_WIN32
#endif

    struct ProcessMetrics
    {
        std::uint16_t threadCount {};
        ExecutionMetrics executionMetrics;
        ProcessMemoryState memoryState;
        MemoryEventCounters memoryEvents;
#if DORY_PLATFORM_LINUX
        EventFileDescriptors eventFileDescriptors;
#elif DORY_PLATFORM_WIN32
#endif
    };

    struct TaskMetrics
    {
        ExecutionMetrics executionMetrics;
        MemoryEventCounters memoryEvents;

#if DORY_PLATFORM_LINUX
        EventFileDescriptors eventFileDescriptors;
#elif DORY_PLATFORM_WIN32
#endif
    };

    struct PerformanceMetricsSettings
    {
        bool tlbMisses{};
        bool wallClock{};
        bool cpuTimeKernel{};
        bool cpuTimeUser{};
        bool minorPageFaults{};
        bool majorPageFaults{};
    };
}
