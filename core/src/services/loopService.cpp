#include <dory/core/registry.h>
#include <dory/core/services/loopService.h>
#include <dory/core/services/iPipelineService.h>
#include <chrono>
#include <spdlog/fmt/fmt.h>

namespace dory::core::services
{
    using namespace core;
    using namespace resources;
    using namespace entities;
    using namespace scene;
    using namespace repositories;
    using namespace services;
    using namespace std::chrono;

    void busyWait(std::chrono::milliseconds duration) {
        // Record the start time
        auto start = std::chrono::high_resolution_clock::now();

        // Keep looping until the specified duration has elapsed
        while (std::chrono::high_resolution_clock::now() - start < duration) {
            // Optionally, do something lightweight here if needed
        }
    }

    LoopService::LoopService(Registry& registry):
        _registry(registry)
    {}

    void LoopService::startLoop(resources::DataContext& context)
    {
        auto pipelineService = _registry.get<IPipelineService>();
        if(pipelineService)
        {
            pipelineService->initialize(context);
        }

        {
            isStop = false;

            auto fpsAccumulator = nanoseconds{0};
            const auto fpsInterval = seconds{1};
            std::size_t frameCounter = 1;
            std::size_t fps = 0;

            //core::resources::profiling::startNewCapture(context.profiling, context.profiling.captureIdCounter++, 100);

            high_resolution_clock::time_point lastTimestamp = high_resolution_clock::now();

            while(!isStop)
            {
                auto currentTimestamp = high_resolution_clock::now();
                auto frameTime = duration_cast<nanoseconds>(currentTimestamp - lastTimestamp);
                lastTimestamp = currentTimestamp;

                profiling::popTimeSlice(context.profiling);

                //Analyze and drop completed Capture
                //TODO: move capture analysis to a pipeline node
                if(auto* capture = profiling::getCurrentCapture(context.profiling))
                {
                    if(capture->done)
                    {
                        if(auto profilingService = _registry.get<IProfilingService>())
                        {
                            profilingService->analyze(*capture);
                        }
                        profiling::removeCurrentCapture(context.profiling);

                        core::resources::profiling::startNewCapture(context.profiling, context.profiling.captureIdCounter++, 100);
                    }
                    else
                    {
                        if(auto* frame = profiling::addNewFrame(*capture, frameCounter))
                        {
                            profiling::pushTimeSlice(*frame, std::string{ profiling::Profiling::frameRootTimeSlice });
                        }
                    }
                }

                //profiling::pushTimeSlice(context.profiling, "wait");
                //busyWait(std::chrono::milliseconds{ 5 });
                //profiling::popTimeSlice(context.profiling);

                if (frameTime > milliseconds(250)) {
                    frameTime = milliseconds(250);
                }

                //frameTime = duration_cast<nanoseconds>(generic::model::TimeSpan(1.f / 60.f));

                fpsAccumulator += frameTime;
                while (fpsAccumulator >= fpsInterval)
                {
                    fpsAccumulator = fpsAccumulator - fpsInterval;

                    if(auto logger = _registry.get<ILogService>())
                    {
                        logger->information(fmt::format("FPS: {}", fps));
                        if(auto* capture = core::resources::profiling::getCurrentCapture(context.profiling))
                        {
                            capture->done = true;
                        }
                    }
                    fps = 0;
                }

                if(auto pipeline = _registry.get<IPipelineService>()) //it can be hot-swapped, this is why reload it each frame
                {
                    profiling::pushTimeSlice(context.profiling, "update");
                    pipeline->update(context, frameTime);
                    profiling::popTimeSlice(context.profiling);
                }

                frameCounter++;
                fps++;
            }
        }

        pipelineService = _registry.get<IPipelineService>();
        if(pipelineService)
        {
            pipelineService->stop(context);
        }
    }

    void LoopService::endLoop()
    {
        isStop = true;
    }
}