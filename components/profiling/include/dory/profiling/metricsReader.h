#pragma once

#include "metrics.h"

namespace dory::profiling
{
    class MetricsReader
    {
    public:
        static void startMetricsRecording(ProcessMetrics& metrics);
        static void completeMetricsRecording(ProcessMetrics& metrics);
        static void startMetricsRecording(TaskMetrics& metrics);
        static void readMetrics(TaskMetrics& metrics);
        static void stopMetricsRecording(TaskMetrics& metrics);
    };
}