#pragma once

#include <dory/macros/assert.h>

namespace dory::memory::profilers
{
    /*
    * Allocation Profiler which is aggregating allocation/deallocation statistics
    */
    template<std::size_t SizeClassCount>
    class MemoryClassAuditProfiler
    {
    public:
        struct SizeClassStatistic
        {
            std::size_t slotSize {};
            std::size_t slotsPerChunk {};
            std::size_t freeCount {};
            std::size_t allocCount {};
        };

        struct GeneralClassStatistic
        {
            std::size_t allocatedSize {};
            std::size_t freedSize {};
            std::size_t freeCount {};
            std::size_t allocCount {};
        };

    private:
        std::array<SizeClassStatistic, SizeClassCount> _classStatistics;
        GeneralClassStatistic _generalClassStatistic;

    public:
        explicit MemoryClassAuditProfiler(std::array<memory::MemorySizeClass, SizeClassCount>& sizeClasses)
        {
            for(std::size_t classIndex = 0; classIndex < SizeClassCount; ++classIndex)
            {
                auto& sizeClass = sizeClasses[classIndex];
                auto& classStatistic = _classStatistics[classIndex];
                classStatistic.slotSize = sizeClass.size;
                classStatistic.slotsPerChunk = sizeClass.slotsPerChunk;
            }
        }

        void traceSlotAlloc(void* ptr, std::size_t size, std::size_t slotSize, std::size_t classIndex)
        {
            assert::debug(classIndex < SizeClassCount, "Size class is out of range");

            auto& classStatistic = _classStatistics[classIndex];
            ++classStatistic.allocCount;
        }

        void traceSlotFree(void* ptr, std::size_t slotSize, std::size_t classIndex)
        {
            assert::debug(classIndex < SizeClassCount, "Size class is out of range");

            auto& classStatistic = _classStatistics[classIndex];
            ++classStatistic.freeCount;
        }

        void traceLargeAlloc(void* ptr, std::size_t size)
        {
            _generalClassStatistic.allocatedSize += size;
            ++_generalClassStatistic.allocCount;
        }

        void traceLargeFree(void* ptr, std::size_t size)
        {
            _generalClassStatistic.freedSize += size;
            ++_generalClassStatistic.freeCount;
        }
    };
}