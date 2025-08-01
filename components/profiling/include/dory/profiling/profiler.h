#pragma once

#include <dory/macros/utility.h>
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#include <source_location>

#define DORY_ZONE_NAMED(var, name) \
static const ___tracy_source_location_data DORY_CONCAT(TracyLoc_, __LINE__) = { name, __func__, __FILE__, (uint32_t)__LINE__, 0 }; \
dory::profiling::Zone var(&DORY_CONCAT(TracyLoc_, __LINE__))

#define DORY_ZONE_VAR(name) \
DORY_ZONE_NAMED(name, #name)

#define DORY_ZONE(name) \
DORY_ZONE_NAMED(DORY_CONCAT(TracyZone_, __LINE__), name)

namespace dory::profiling
{
    inline void setThreadName(const char* name)
    {
        tracy::SetThreadName(name);
    }

    inline void traceFrameMark()
    {
        FrameMark;
    }

    inline void traceAllocation(const void* ptr, const std::size_t size)
    {
        TracyAlloc(ptr, size);
    }

    inline void traceDeallocation(const void* ptr)
    {
        TracyFree(ptr);
    }

    struct Zone
    {
        explicit Zone(const ___tracy_source_location_data* srcLoc, const bool active = true):
            _ctx(___tracy_emit_zone_begin_callstack(srcLoc, TRACY_CALLSTACK, active))
        {}

        ~Zone()
        {
            end();
        }

        Zone(const Zone&) = delete;
        Zone& operator=(const Zone&) = delete;

        Zone(Zone&& other) noexcept:
            _ctx(other._ctx)
        {
            other._ctx = {};
        }

        Zone& operator=(Zone&& other) noexcept
        {
            if (this != &other)
            {
                TracyCZoneEnd(_ctx);
                _ctx = other._ctx;
                other._ctx = {};
            }
            return *this;
        }

        void end()
        {
            if(!_finished)
            {
                ___tracy_emit_zone_end(_ctx);
                _finished = true;
            }
        }

    private:
        TracyCZoneCtx _ctx;
        bool _finished {};
    };

    struct TraceZone
    {
        explicit TraceZone(const ___tracy_source_location_data* srcLoc, const bool active = true):
            _ctx(___tracy_emit_zone_begin_callstack(srcLoc, TRACY_CALLSTACK, active))
        {}

        ~TraceZone()
        {
            end();
        }

        TraceZone(const TraceZone&) = delete;
        TraceZone& operator=(const TraceZone&) = delete;

        TraceZone(TraceZone&& other) noexcept:
            _ctx(other._ctx),
            _finished(other._finished)
        {
            other._ctx = {};
            other._finished = true;
        }

        TraceZone& operator=(TraceZone&& other) noexcept
        {
            if (this != &other)
            {
                TracyCZoneEnd(_ctx);
                _ctx = other._ctx;
                _finished = other._finished;
                other._ctx = {};
            }
            return *this;
        }

        void end()
        {
            if(!_finished)
            {
                ___tracy_emit_zone_end(_ctx);
                _finished = true;
            }
        }

        template<typename TTranslationUnit, std::size_t LineNumber>
        static TraceZone get(const char* name, const std::source_location& loc = std::source_location::current())
        {
            static const ___tracy_source_location_data tracyLocation {name, loc.function_name(), loc.file_name(), loc.line(), 0};
            TraceZone zone {&tracyLocation};

            return zone;
        }

    private:
        TracyCZoneCtx _ctx;
        bool _finished {};
    };

    consteval std::size_t ln(const std::source_location& location = std::source_location::current())
    {
        return location.line();
    }

    template<typename TTranslationUnit, std::size_t Line>
    auto zone(const char* name, const std::source_location& location = std::source_location::current())
    {
        return TraceZone::get<TTranslationUnit, Line>(name, location);
    }
}
