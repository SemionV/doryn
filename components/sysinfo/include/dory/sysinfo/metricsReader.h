#pragma once

#include <dory/sysinfo/metrics.h>

namespace dory::sysinfo
{
    class MetricsReader
    {
    public:
        static void getProcessMetrics(ProcessMetrics& metrics);
    };
}