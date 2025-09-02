#pragma once

#include <source_location>
#include "dory/macros/build.h"
#include "dory/macros/utility.h"

#ifdef DORY_PROFILING_ON
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#endif

#ifdef DORY_GPU_PROFILING_ON
#ifndef DORY_OPENGL_INCLUDED
#include <glad/gl.h>
#define DORY_OPENGL_INCLUDED
#endif
#include <tracy/TracyOpenGL.hpp>
#endif

namespace dory::profiling
{
#ifdef DORY_PROFILING_ON
    DORY_DLL_API void startProfiler();
    DORY_DLL_API void stopProfiler();

    DORY_DLL_API void setThreadName(const char* name);
    DORY_DLL_API void traceFrameMark();
    DORY_DLL_API void traceFrameStart(const char* name);
    DORY_DLL_API void traceFrameEnd(const char* name);
    DORY_DLL_API void traceAllocation(const void* ptr, std::size_t size, const char* poolName);
    DORY_DLL_API void traceDeallocation(const void* ptr, const char* poolName);
    DORY_DLL_API void traceMessageStack(const char* message, std::size_t messageSize, std::size_t stackDepth = 10);
    DORY_DLL_API void shutdown();
    DORY_DLL_API void setProfilerReady();
    DORY_DLL_API void setProfilerNotReady();
    DORY_DLL_API bool isProfilerReady();
#endif

#ifdef DORY_GPU_PROFILING_ON
    DORY_DLL_API void registerOpenGLContext(const char* contextName, std::size_t nameSize);
    DORY_DLL_API void collectGpuTraces();
#endif

#ifdef DORY_PROFILING_ON
    struct DORY_DLL_API TraceZone
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
#define DORY_TRACE_FRAME_START(name) dory::profiling::traceFrameStart(name);
#define DORY_TRACE_FRAME_END(name) dory::profiling::traceFrameEnd(name);
#define DORY_TRACE_THREAD_NAME(name) dory::profiling::setThreadName(name);
#define DORY_TRACE_MEM_ALLOC(ptr, size, poolName) dory::profiling::traceAllocation(ptr, size, poolName);
#define DORY_TRACE_MEM_FREE(ptr, poolName) dory::profiling::traceDeallocation(ptr, poolName);
#define DORY_TRACE_MESSAGE_STACK(message, messageSize, stackDepth) dory::profiling::traceMessageStack(message, messageSize, stackDepth);
#define DORY_TRACE_START() dory::profiling::startProfiler();
#define DORY_TRACE_SHUTDOWN() dory::profiling::stopProfiler();
#define DORY_TRACE_SET_PROFILER_READY() dory::profiling::setProfilerReady();
#define DORY_TRACE_SET_PROFILER_NOT_READY() dory::profiling::setProfilerNotReady();
#define DORY_TRACE_IS_PROFILER_READY dory::profiling::isProfilerReady()

#else

#define DORY_TRACE_ZONE(name) DORY_NOOP
#define DORY_TRACE_ZONE_NAMED(varName, name) DORY_NOOP
#define DORY_TRACE_ZONE_END(varName) DORY_NOOP
#define DORY_TRACE_FRAME_MARK DORY_NOOP
#define DORY_TRACE_FRAME_START(name) DORY_NOOP
#define DORY_TRACE_FRAME_END(name) DORY_NOOP
#define DORY_TRACE_THREAD_NAME(name) DORY_NOOP;
#define DORY_TRACE_MEM_ALLOC(ptr, size, poolName) DORY_NOOP;
#define DORY_TRACE_MEM_FREE(ptr, poolName) DORY_NOOP;
#define DORY_TRACE_MESSAGE_STACK(message, messageSize, stackDepth) DORY_NOOP;
#define DORY_TRACE_START() DORY_NOOP;
#define DORY_TRACE_SHUTDOWN() DORY_NOOP;
#define DORY_TRACE_SET_PROFILER_READY() DORY_NOOP;
#define DORY_TRACE_SET_PROFILER_NOT_READY() DORY_NOOP;
#define DORY_TRACE_IS_PROFILER_READY false

#endif

#ifdef DORY_GPU_PROFILING_ON
#define DORY_TRACE_GPU_ZONE(name) \
static const tracy::SourceLocationData DORY_CONCAT(_tracyGpuLoc_, __LINE__) = { \
name, __FUNCTION__, __FILE__, static_cast<uint32_t>(__LINE__), 0 \
}; \
tracy::GpuCtxScope DORY_CONCAT(_tracyGpuZone_, __LINE__)(&DORY_CONCAT(_tracyGpuLoc_, __LINE__), TRACY_CALLSTACK, true)

#define DORY_TRACE_GPU_CONTEXT(name, size) dory::profiling::registerOpenGLContext(name, size);
#define DORY_TRACE_GPU_COLLECT() dory::profiling::collectGpuTraces();

#else

#define DORY_TRACE_GPU_CONTEXT(name, size) DORY_NOOP;
#define DORY_TRACE_GPU_COLLECT() DORY_NOOP;
#define DORY_TRACE_GPU_ZONE(name) DORY_NOOP;

#endif
