#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <tracy/TracyOpenGL.hpp>
#include <dory/profiling/profiler.h>
#include <dory/macros/build.h>

namespace dory::profiling
{
    void startProfiler()
    {
        tracy::StartupProfiler();
    }

    void stopProfiler()
    {
        tracy::ShutdownProfiler();
    }

    void setThreadName(const char* name)
    {
        tracy::SetThreadName(name);
    }

    void traceFrameMark()
    {
        tracy::Profiler::SendFrameMark(nullptr);
    }

    void traceFrameStart(const char* name)
    {
        FrameMarkStart(name);
    }

    void traceFrameEnd(const char* name)
    {
        FrameMarkStart(name);
    }

    void traceAllocation(const void* ptr, const std::size_t size, const char* poolName)
    {
        //tracy::Profiler::MemAllocCallstack(ptr, size, 10, false);
        tracy::Profiler::MemAllocCallstackNamed(ptr, size, 10, false, poolName);
    }

    void traceDeallocation(const void* ptr, const char* poolName)
    {
        //tracy::Profiler::MemFreeCallstack(ptr, 10, false);
        tracy::Profiler::MemFreeCallstackNamed(ptr, 10, false, poolName);
    }

    void shutdown()
    {
        traceFrameMark();
    }

    void registerOpenGLContext(const char* contextName, const std::size_t nameSize)
    {
        TracyGpuContextName(contextName, nameSize);
    }

    TraceZone::TraceZone(const ___tracy_source_location_data* srcLoc, const bool active):
    _ctx(___tracy_emit_zone_begin_callstack(srcLoc, TRACY_CALLSTACK, active))
    {}

    TraceZone::~TraceZone()
    {
        end();
    }

    TraceZone::TraceZone(TraceZone&& other) noexcept:
        _ctx(other._ctx),
        _finished(other._finished)
    {
        other._ctx = {};
        other._finished = true;
    }

    TraceZone& TraceZone::operator=(TraceZone&& other) noexcept
    {
        if (this != &other)
        {
            end();
            _ctx = other._ctx;
            _finished = other._finished;
            other._ctx = {};
        }
        return *this;
    }

    void TraceZone::end()
    {
        if(!_finished)
        {
            ___tracy_emit_zone_end(_ctx);
            _finished = true;
        }
    }
}
