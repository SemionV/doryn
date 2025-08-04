#pragma once

#include <source_location>
#include "dory/macros/build.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>
#endif

namespace dory::profiling
{
    DORY_DLLEXPORT void setThreadName(const char* name);
    DORY_DLLEXPORT void traceFrameMark();
    DORY_DLLEXPORT void traceAllocation(const void* ptr, const std::size_t size, const char* poolName);
    DORY_DLLEXPORT void traceDeallocation(const void* ptr, const char* poolName);

    struct TraceZone
    {
#ifdef TRACY_ENABLE
        explicit TraceZone(const ___tracy_source_location_data* srcLoc, const bool active = true);
#else
        TraceZone() = default;
#endif

#ifdef TRACY_ENABLE
        ~TraceZone();
#else
        ~TraceZone() = default;
#endif

        TraceZone(const TraceZone&) = delete;
        TraceZone& operator=(const TraceZone&) = delete;

        TraceZone(TraceZone&& other) noexcept;
        TraceZone& operator=(TraceZone&& other) noexcept;

        void end();

#ifdef TRACY_ENABLE
        template<typename TTranslationUnit, std::size_t LineNumber>
        static TraceZone get(const char* name, const std::source_location& loc = std::source_location::current())
        {
            static const ___tracy_source_location_data tracyLocation {name, loc.function_name(), loc.file_name(), loc.line(), 0};
            TraceZone zone {&tracyLocation};

            return zone;
        }
#else
        template<typename TTranslationUnit, std::size_t LineNumber>
        static TraceZone get(const char* name, const std::source_location& loc = std::source_location::current())
        {
            return TraceZone{};
        }
#endif

    private:
#ifdef TRACY_ENABLE
        TracyCZoneCtx _ctx;
        bool _finished {};
#endif
    };

    consteval std::size_t ln(const std::source_location& location = std::source_location::current())
    {
        return location.line();
    }

    template<typename TTranslationUnit, std::size_t Line>
    auto zone(const char* name, const std::source_location& location = std::source_location::current())
    {
        //In case when profiling is disabled, the TraceZone object will be optimized and should not make any pressure on the caller function stack
        return TraceZone::get<TTranslationUnit, Line>(name, location);
    }
}
