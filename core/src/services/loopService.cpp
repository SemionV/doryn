#include <dory/core/registry.h>
#include <dory/core/services/loopService.h>
#include <dory/core/services/iPipelineService.h>
#include <dory/generic/model.h>
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

            const auto fixedDeltaTime = nanoseconds{16666667 * 2};
            auto accumulator = nanoseconds{0};
            auto fpsAccumulator = nanoseconds{0};
            const auto fpsInterval = seconds{1};
            generic::model::TimeSpan timeStep(generic::model::UnitScale::Nano);
            std::size_t frameCounter = 1;

            steady_clock::time_point lastTimestamp = steady_clock::now();

            SceneViewStateSet sceneStatesA;
            SceneViewStateSet sceneStatesB;

            const std::atomic viewStateWrite { &sceneStatesA };
            /*std::atomic<SceneViewStateSet*> viewStateRead { &sceneStatesB };*/

            profiling::Frame* frame = nullptr;

            while(!isStop)
            {
                steady_clock::time_point currentTimestamp = steady_clock::now();
                auto frameTime = duration_cast<nanoseconds>(currentTimestamp - lastTimestamp);
                lastTimestamp = currentTimestamp;

                if(frame)
                {
                    frame->end = currentTimestamp;
                    frame = nullptr;
                }

                if (frameTime > milliseconds(250)) {
                    frameTime = milliseconds(250);
                    accumulator = nanoseconds{0};
                }

                accumulator += frameTime;
                fpsAccumulator += frameTime;

                while (fpsAccumulator >= fpsInterval)
                {
                    fpsAccumulator = fpsAccumulator - fpsInterval;
                }

                if(context.profiling.captureFrameStatistics)
                {
                    frame = &context.profiling.frames.emplace_back();
                    frame->id = frameCounter;
                    frame->begin = currentTimestamp;
                    if(context.profiling.frames.size() > profiling::Profiling::maxFramesCapture)
                    {
                        context.profiling.frames.pop_back();
                    }
                }

                pipelineService = _registry.get<IPipelineService>();
                auto viewService = _registry.get<IViewService>();
                if(pipelineService && viewService) //it can be hot-swapped, this is why reload it each frame
                {
                    constexpr int maxUpdatesPerFrame = 5;
                    int updates = 0;
                    while (accumulator >= fixedDeltaTime && updates < maxUpdatesPerFrame) {
                        timeStep.duration = fixedDeltaTime.count();
                        pipelineService->update(context, timeStep);

                        const auto viewStates = viewStateWrite.load();
                        viewService->updateViewsState(*viewStates);

                        accumulator -= fixedDeltaTime;
                        updates++;
                        if(frame)
                        {
                            frame->updatesCount = updates;
                        }
                    }
                }

                /*auto viewStates = viewStateRead.exchange(viewStateWrite.load());
                viewStateWrite.store(viewStates);*/

                viewService = _registry.get<IViewService>();
                if(viewService)
                {
                    float alpha = static_cast<float>(accumulator.count()) / static_cast<float>(fixedDeltaTime.count());
                    alpha = glm::clamp(alpha, 0.0f, 1.0f);

                    const auto viewStates = viewStateWrite.load();
                    if(frame)
                    {
                        frame->viewStates.push_back(*viewStates);
                        frame->alpha = alpha;
                    }
                    viewService->updateViews(*viewStates, alpha, context.profiling);
                }

                frameCounter++;
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

    /*void LoopService::printProfilingInfo(const profiling::Profiling& profiling) const
    {
        if(auto logger = _registry.get<ILogService>())
        {
            std::size_t framesWithUpdatesCount {};
            for(const auto& frame : profiling.frames)
            {
                if(frame.updatesCount != 0)
                {
                    framesWithUpdatesCount++;
                }
            }

            logger->information(fmt::format("FPS: {0}, u: {1}",
                profiling.frames.size(),
                framesWithUpdatesCount));
        }

        if(profiling.frames.size() < 300)
        {
            printProfilingDetailedInfo(profiling);
        }
    }

    void LoopService::printProfilingDetailedInfo(const resources::profiling::Profiling& profiling) const
    {
        std::chrono::nanoseconds duration {};
        for(const auto& frame : profiling.frames)
        {
            printFrameInfo(frame);
            duration += frame.duration;
        }

        if(auto logger = _registry.get<ILogService>())
        {
            logger->information(fmt::format("total duration: {0}ms",
                std::chrono::duration_cast<milliseconds>(duration).count()));
        }
    }

    void LoopService::printFrameInfo(const resources::profiling::Frame& frame) const
    {
        if(auto logger = _registry.get<ILogService>())
        {
            logger->information(fmt::format("frame: {0}ms, updates: {1}, alpha: {2}, fbb: {3}, rb: {4}, db: {5}",
                std::chrono::duration_cast<milliseconds>(frame.duration).count(),
                frame.updatesCount,
                frame.alpha,
                frame.frameBufferBinding,
                frame.readFrameBufferIndex ? "back" : "front",
                frame.drawFrameBufferIndex ? "back" : "front"));
        }
    }*/
}