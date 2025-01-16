#pragma once

#include <dory/generic/baseTypes.h>

namespace dory::core::services
{
    class IProfilingService: public generic::Interface
    {
    public:
        virtual void analyze(const resources::profiling::Profiling& profiling) = 0;
    };
}