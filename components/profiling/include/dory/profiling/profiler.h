#pragma once

#include <source_location>
#include "dory/macros/build.h"
#include "dory/macros/utility.h"

#ifdef DORY_PROFILING_ON
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#endif

namespace dory::profiling
{
#ifdef DORY_PROFILING_ON
    DORY_DLLEXPORT void setThreadName(const char* name);
    DORY_DLLEXPORT void traceFrameMark();
    DORY_DLLEXPORT void traceAllocation(const void* ptr, std::size_t size, const char* poolName);
    DORY_DLLEXPORT void traceDeallocation(const void* ptr, const char* poolName);
    DORY_DLLEXPORT void shutdown();
#endif

#ifdef DORY_PROFILING_ON
    struct TraceZone
    {
        explicit TraceZone(const ___tracy_source_location_data* srcLoc, bool active = true);
        ~TraceZone();

        TraceZone(const TraceZone&) = delete;
        TraceZone& operator=(const TraceZone&) = delete;

        TraceZone(TraceZone&& other) noexcept;
        TraceZone& operator=(TraceZone&& other) noexcept;

        void end();

    private:
        TracyCZoneCtx _ctx;
        bool _finished {};
    };
#endif
}

#ifdef DORY_PROFILING_ON

#define DORY_TRACE_ZONE(name) \
static const ___tracy_source_location_data DORY_CONCAT(_tracyLoc_, __LINE__) = { \
name, __FUNCTION__, __FILE__, __LINE__, 0 \
}; \
TraceZone DORY_CONCAT(_traceZone_, __LINE__) { &DORY_CONCAT(_tracyLoc_, __LINE__) }

#define DORY_TRACE_ZONE_NAMED(varName, name) \
static const ___tracy_source_location_data DORY_CONCAT(_tracyLoc_, __LINE__) = { \
name, __FUNCTION__, __FILE__, __LINE__, 0 \
}; \
::dory::profiling::TraceZone varName { &DORY_CONCAT(_tracyLoc_, __LINE__) }

#define DORY_TRACE_ZONE_END(varName) varName.end();

#define DORY_TRACE_FRAME_MARK dory::profiling::traceFrameMark();
#define DORY_TRACE_THREAD_NAME(name) dory::profiling::setThreadName(name);
#define DORY_TRACE_ALLOC(ptr, size, poolName) dory::profiling::traceAllocation(ptr, size, poolName);
#define DORY_TRACE_FREE(ptr, poolName) dory::profiling::traceDeallocation(ptr, poolName);
#define DORY_TRACE_SHUTDOWN() dory::profiling::shutdown();

#else

#define DORY_TRACE_ZONE(name) DORY_NOOP
#define DORY_TRACE_ZONE_NAMED(varName, name) DORY_NOOP
#define DORY_TRACE_ZONE_END(varName) DORY_NOOP
#define DORY_TRACE_FRAME_MARK DORY_NOOP
#define DORY_TRACE_THREAD_NAME(name) DORY_NOOP;
#define DORY_TRACE_ALLOC(ptr, size, poolName) DORY_NOOP;
#define DORY_TRACE_FREE(ptr, poolName) DORY_NOOP;
#define DORY_TRACE_SHUTDOWN() DORY_NOOP;

#endif
