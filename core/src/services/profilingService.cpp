#include <dory/core/registry.h>
#include <dory/core/services/profilingService.h>
#include <chrono>
#include <spdlog/fmt/fmt.h>

namespace dory::core::services
{
    using namespace resources;
    using namespace profiling;
    using namespace std::chrono;

    template <typename Duration>
    Duration absoluteDuration(const Duration& d)
{
        return Duration(std::abs(d.count()));
    }

    ProfilingService::ProfilingService(Registry& registry): DependencyResolver(registry)
    {}

    void ProfilingService::analyze(const Profiling& profiling)
    {
        if(!profiling.frames.empty())
        {
            nanoseconds totalDuration = {};
            auto defaultTimePoint = steady_clock::time_point();
            for(const auto& frame : profiling.frames)
            {
                if(frame.end != defaultTimePoint)
                {
                    const auto duration = duration_cast<nanoseconds>(frame.end - frame.begin);
                    totalDuration += duration;
                }
            }

            const auto averageFrameDuration = totalDuration / profiling.frames.size();
            const auto maxDeviationTime = std::chrono::duration_cast<nanoseconds>(averageFrameDuration * 0.3f);
            auto nonAverageFrames = std::vector<std::size_t>{};

            for(std::size_t i = 0; i < profiling.frames.size(); ++i)
            {
                const auto& frame = profiling.frames[i];

                if(frame.end != defaultTimePoint)
                {
                    const auto duration = duration_cast<nanoseconds>(frame.end - frame.begin);
                    const auto frameDeviationTime = absoluteDuration(duration_cast<nanoseconds>(averageFrameDuration - duration));

                    if(frameDeviationTime > maxDeviationTime)
                    {
                        nonAverageFrames.push_back(i);
                    }
                }
            }

            if(auto logger = _registry.get<ILogService>())
            {
                std::string message {};

                auto averageFrameTimeMs = duration<double, std::milli>(averageFrameDuration).count();
                message = fmt::format("Avg. frame time: {0} ms\n", averageFrameTimeMs);
                message += fmt::format("Non avg. frames({0}):\n", nonAverageFrames.size());

                for(const auto i : nonAverageFrames)
                {
                    const auto& frame = profiling.frames[i];
                    const auto duration = duration_cast<nanoseconds>(frame.end - frame.begin);
                    auto durationMs = std::chrono::duration<double, std::milli>(duration).count();
                    message += fmt::format("{0}: {1}\n", i, durationMs);
                }

                logger->information(message);
            }
        }
    }
}
