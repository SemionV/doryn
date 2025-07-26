#include <fstream>
#include <dory/profiling/metricsReader.h>

#if DORY_PLATFORM_LINUX
#include <sys/resource.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <asm/unistd_64.h>

void readProcSelfStatus(dory::profiling::ProcessMetrics& processMetrics) {
    std::ifstream f("/proc/self/status");
    std::string line;
    dory::profiling::ProcessMemoryState& ms = processMetrics.memoryState;
    while (std::getline(f, line))
    {
        if (sscanf(line.c_str(), "VmSize: %zu kB", &ms.virtualMemorySize)==1) ms.virtualMemorySize *= 1024;
        if (sscanf(line.c_str(), "VmRSS: %zu kB", &ms.residentSetSize)==1) ms.residentSetSize *= 1024;
        if (sscanf(line.c_str(), "VmData: %zu kB", &ms.dataSize)==1) ms.dataSize *= 1024;
        if (sscanf(line.c_str(), "VmStk: %zu kB", &ms.stackSize)==1) ms.stackSize *= 1024;
        if (sscanf(line.c_str(), "Threads: %hu", &processMetrics.threadCount)==1){}
    }
}

void readProcSelfSmaps(dory::profiling::ProcessMemoryState &ms)
{
    std::ifstream f("/proc/self/smaps");
    std::string line; size_t private_kb=0, shared_kb=0, dirty_kb=0, mapCount=0;
    while (std::getline(f, line))
    {
        if (line.empty()) continue;
        if (line[0] == '/') mapCount++;
        if (sscanf(line.c_str(), "Private_Dirty: %zu kB", &private_kb)==1) ms.privateMemoryUsage += private_kb*1024;
        if (sscanf(line.c_str(), "Shared_Clean: %zu kB", &shared_kb)==1) ms.sharedMemoryUsage += shared_kb*1024;
        if (sscanf(line.c_str(), "Shared_Dirty: %zu kB", &shared_kb)==1) ms.sharedMemoryUsage += shared_kb*1024;
        if (sscanf(line.c_str(), "Dirty: %zu kB", &dirty_kb)==1) ms.dirtyPages += dirty_kb*1024;
    }
    ms.memoryMapCount = mapCount;
}

void readRusageForProcess(dory::profiling::MemoryEventCounters& memoryEvents)
{
    rusage ru{};
    getrusage(RUSAGE_SELF, &ru);
    memoryEvents.minorPageFaults = ru.ru_minflt;
    memoryEvents.majorPageFaults = ru.ru_majflt;
    memoryEvents.pageSwapCount = ru.ru_majflt;
}

int openPerfTLB()
{
    perf_event_attr attr{};
    attr.type = PERF_TYPE_HW_CACHE;
    attr.config = (PERF_COUNT_HW_CACHE_DTLB)|(PERF_COUNT_HW_CACHE_OP_READ<<8)|(PERF_COUNT_HW_CACHE_RESULT_MISS<<16);
    return syscall(__NR_perf_event_open, &attr, 0, -1, -1, 0);
}

size_t readPerfCounter(int fd)
{
    uint64_t count;
    read(fd, &count, sizeof(count));
    close(fd);
    return count;
}

//TODO: generalize the function for more events, but be careful, because CPUs have very limited number of counters which can be used simultaneously(just a few events in real time)
void startPerfTLB(dory::profiling::MemoryEventCounters &ev, dory::profiling::ExecutionMetrics& executionMetrics)
{
    const int fd = ev._tlbMissesFD = openPerfTLB();
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
}

void completePerfTLB(dory::profiling::MemoryEventCounters &ev, dory::profiling::ExecutionMetrics& executionMetrics)
{
    const int fd = ev._tlbMissesFD;
    if(fd >= 0)
    {
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        ev.tlbMisses = readPerfCounter(fd);
    }
}

void startMetricsRecordingPlatform(dory::profiling::ProcessMetrics& metrics)
{
    startPerfTLB(metrics.memoryEvents, metrics.executionMetrics);
}

void completeMetricsRecordingPlatform(dory::profiling::ProcessMetrics& metrics)
{
    readProcSelfStatus(metrics);
    readProcSelfSmaps(metrics.memoryState);
    readRusageForProcess(metrics.memoryEvents);
    completePerfTLB(metrics.memoryEvents, metrics.executionMetrics);
}

void startMetricsRecordingPlatform(dory::profiling::TaskMetrics& metrics)
{
    startPerfTLB(metrics.memoryEvents, metrics.executionMetrics);
}

void completeMetricsRecordingPlatform(dory::profiling::TaskMetrics& metrics)
{

}
#elif DORY_PLATFORM_WIN32
#endif

namespace dory::profiling
{
    void MetricsReader::startMetricsRecording(ProcessMetrics& metrics)
    {
        //TODO: split snapshot metrics from counters(like overall memory used versus page faults count)
        startMetricsRecordingPlatform(metrics);
    }

    void MetricsReader::completeMetricsRecording(ProcessMetrics& metrics)
    {
        //TODO: do not close the performance counters, just read the current values and introduce a separate method to close the counters
        completeMetricsRecordingPlatform(metrics);
    }

    void MetricsReader::startMetricsRecording(TaskMetrics& metrics)
    {
        startMetricsRecordingPlatform(metrics);
    }

    void MetricsReader::completeMetricsRecording(TaskMetrics& metrics)
    {
        completeMetricsRecordingPlatform(metrics);
    }
}
