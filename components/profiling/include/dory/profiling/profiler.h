#pragma once

#include <dory/macros/utility.h>
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>

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

    inline void frameMark()
    {
        FrameMark;
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
}
