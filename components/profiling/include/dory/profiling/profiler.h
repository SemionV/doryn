#pragma once

#include <tracy/Tracy.hpp>

namespace dory::profiling
{
    inline void setThreadName(const char* name)
    {
        tracy::SetThreadName(name);
    }
}
