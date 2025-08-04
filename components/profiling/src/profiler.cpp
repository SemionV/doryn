#include <dory/profiling/profiler.h>
#include <dory/macros/build.h>

namespace dory::profiling
{
    void setThreadName(const char* name)
    {
#ifdef TRACY_ENABLE
        tracy::SetThreadName(name);
#endif
    }

    void traceFrameMark()
    {
#ifdef TRACY_ENABLE
        tracy::Profiler::SendFrameMark(nullptr);
#endif
    }

    void traceAllocation(const void* ptr, const std::size_t size, const char* poolName)
    {
#ifdef TRACY_ENABLE
        //tracy::Profiler::MemAllocCallstack(ptr, size, 10, false);
        tracy::Profiler::MemAllocCallstackNamed(ptr, size, 10, false, poolName);
#endif
    }

    void traceDeallocation(const void* ptr, const char* poolName)
    {
#ifdef TRACY_ENABLE
        //tracy::Profiler::MemFreeCallstack(ptr, 10, false);
        tracy::Profiler::MemFreeCallstackNamed(ptr, 10, false, poolName);
#endif
    }

#ifdef TRACY_ENABLE
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
#else
        TraceZone::TraceZone(TraceZone&& other) noexcept
        {}

        TraceZone& TraceZone::operator=(TraceZone&& other) noexcept
        {
            return *this;
        }

        void TraceZone::end()
        {}
#endif
}
