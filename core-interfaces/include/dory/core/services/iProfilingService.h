#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/profiling.h>

namespace dory::core::services
{
    class IProfilingService: public generic::Interface
    {
    public:
        virtual void analyze(const resources::profiling::Capture& capture) = 0;
    };
}