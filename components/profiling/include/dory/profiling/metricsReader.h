#pragma once

#include "metrics.h"

namespace dory::profiling
{
    class MetricsReader
    {
    public:
        static void getProcessMetrics(ProcessMetrics& metrics);
    };
}