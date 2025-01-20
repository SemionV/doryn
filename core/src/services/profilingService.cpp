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

    void ProfilingService::analyze(const Capture& capture)
    {
        if(!capture.frames.empty())
        {
            nanoseconds totalDuration = {};

            for(const auto& frame : capture.frames)
            {
                if(const auto* rootTimeSlice = profiling::getRootTimeSlice(frame))
                {
                    const auto duration = duration_cast<nanoseconds>(rootTimeSlice->end - rootTimeSlice->begin);
                    totalDuration += duration;
                }
            }

            const auto averageFrameDuration = totalDuration / capture.frames.size();
            const auto maxDeviationTime = std::chrono::duration_cast<nanoseconds>(averageFrameDuration * 0.2f);
            auto nonAverageFrames = std::vector<std::size_t>{};

            for(std::size_t i = 0; i < capture.frames.size(); ++i)
            {
                const auto& frame = capture.frames[i];

                if(const auto* rootTimeSlice = profiling::getRootTimeSlice(frame))
                {
                    const auto duration = duration_cast<nanoseconds>(rootTimeSlice->end - rootTimeSlice->begin);
                    const auto frameDeviationTime = absoluteDuration(duration_cast<nanoseconds>(averageFrameDuration - duration));

                    if(frameDeviationTime >= maxDeviationTime)
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
                    const auto& frame = capture.frames[i];
                    if(const auto* rootTimeSlice = profiling::getRootTimeSlice(frame))
                    {
                        message += fmt::format("{0}: ", i);

                        std::stack<std::pair<const TimeSlice*, std::size_t>> tree;
                        tree.emplace(rootTimeSlice, 0);

                        while(!tree.empty())
                        {
                            const auto [parentTimeSlice, level] = tree.top();
                            tree.pop();

                            for(std::size_t j = 0; j < level; ++j)
                            {
                                message += "\t";
                            }

                            auto duration = duration_cast<nanoseconds>(parentTimeSlice->end - parentTimeSlice->begin);
                            auto timeSliceDurationMs = std::chrono::duration<double, std::milli>(duration).count();
                            const auto frameDeviationTime = absoluteDuration(duration_cast<nanoseconds>(averageFrameDuration - duration));
                            if(level == 0 && frameDeviationTime >= maxDeviationTime)
                            {
                                const std::string red = "\033[31m";
                                const std::string reset = "\033[0m";
                                message += fmt::format("{}: {}{}{} ms\n", parentTimeSlice->name, red, timeSliceDurationMs, reset);
                            }
                            else
                            {
                                message += fmt::format("{}: {} ms\n", parentTimeSlice->name, timeSliceDurationMs);
                            }

                            for (auto it = parentTimeSlice->subTimeSlices.rbegin(); it != parentTimeSlice->subTimeSlices.rend(); ++it)
                            {
                                tree.emplace(it->get(), level + 1);
                            }
                        }
                    }
                }

                logger->information(message);
            }
        }
    }
}
