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

            const auto fixedDeltaNanos = nanoseconds{16666667 * 2};
            auto accumulator = nanoseconds{0};
            auto fpsAccumulator = nanoseconds{0};
            const auto fpsInterval = seconds{1};
            constexpr unsigned maxFrameSets = 5;
            generic::model::TimeSpan timeStep(generic::model::UnitScale::Nano);

            steady_clock::time_point lastTimestamp = steady_clock::now();

            SceneViewStateSet sceneStatesA;
            SceneViewStateSet sceneStatesB;

            const std::atomic viewStateWrite { &sceneStatesA };
            /*std::atomic<SceneViewStateSet*> viewStateRead { &sceneStatesB };*/

            std::deque<profiling::FrameSet> frameSets;
            auto* currentFrameSet = &frameSets.emplace_back();
            profiling::Frame* previousFrame = nullptr;

            while(!isStop)
            {
                steady_clock::time_point currentTimestamp = steady_clock::now();
                auto frameTime = duration_cast<nanoseconds>(currentTimestamp - lastTimestamp);
                lastTimestamp = currentTimestamp;

                if(previousFrame)
                {
                    previousFrame->duration = frameTime;
                }
                else
                {
                    frameTime = nanoseconds{ 0 };
                }

                if (frameTime > milliseconds(250)) {
                    frameTime = milliseconds(250);
                }

                accumulator += frameTime;
                fpsAccumulator += frameTime;

                if(fpsAccumulator >= fpsInterval)
                {
                    profiling::Frame frame {};
                    if(previousFrame)
                    {
                        frame = *previousFrame;
                        currentFrameSet->frames.pop_front();
                    }
                    printProfilingInfo(*currentFrameSet);
                    if(frameSets.size() == maxFrameSets)
                    {
                        frameSets.pop_back();
                    }
                    currentFrameSet = &frameSets.emplace_front();
                    if(previousFrame)
                    {
                        currentFrameSet->frames.emplace_front(frame);
                    }
                    fpsAccumulator = fpsAccumulator - fpsInterval;
                }
                previousFrame = &currentFrameSet->frames.emplace_front();

                pipelineService = _registry.get<IPipelineService>();
                auto viewService = _registry.get<IViewService>();
                if(pipelineService && viewService) //it can be hot-swapped, this is why reload it each frame
                {
                    while (accumulator >= fixedDeltaNanos) {
                        timeStep.duration = fixedDeltaNanos.count();
                        pipelineService->update(context, timeStep);

                        const auto viewStates = viewStateWrite.load();
                        viewService->updateViewsState(*viewStates);

                        accumulator -= fixedDeltaNanos;
                    }
                }

                /*auto viewStates = viewStateRead.exchange(viewStateWrite.load());
                viewStateWrite.store(viewStates);*/

                viewService = _registry.get<IViewService>();
                if(viewService)
                {
                    float alpha = static_cast<float>(accumulator.count()) / static_cast<float>(fixedDeltaNanos.count());
                    alpha = glm::clamp(alpha, 0.0f, 1.0f);

                    const auto viewStates = viewStateWrite.load();
                    previousFrame->viewStates.push_back(*viewStates);
                    viewService->updateViews(*viewStates, /*alpha*/1.f);
                }
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

    void LoopService::printProfilingInfo(const profiling::FrameSet& frameSet) const
    {
        auto logger = _registry.get<ILogService>();
        if(logger)
        {
            logger->information(fmt::format("FPS: {0}", frameSet.frames.size()));
        }
    }

}