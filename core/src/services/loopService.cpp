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

            while(!isStop)
            {
                steady_clock::time_point currentTimestamp = steady_clock::now();
                auto frameTime = duration_cast<nanoseconds>(currentTimestamp - lastTimestamp);
                lastTimestamp = currentTimestamp;

                profiling::popTimeSlice(context.profiling, currentTimestamp);

                //Analyze and drop completed Capture
                if(auto* capture = profiling::getCurrentCapture(context.profiling))
                {
                    if(capture->done)
                    {
                        if(auto profilingService = _registry.get<IProfilingService>())
                        {
                            profilingService->analyze(*capture);
                        }
                        profiling::removeCurrentCapture(context.profiling);
                    }
                    else
                    {
                        if(auto* frame = profiling::addNewFrame(*capture, frameCounter))
                        {
                            profiling::pushTimeSlice(*frame, std::string{ profiling::Profiling::frameRootTimeSlice }, currentTimestamp);
                        }
                    }
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

                pipelineService = _registry.get<IPipelineService>();
                auto viewService = _registry.get<IViewService>();
                if(pipelineService && viewService) //it can be hot-swapped, this is why reload it each frame
                {
                    profiling::pushTimeSlice(context.profiling, "update", steady_clock::now());

                    constexpr int maxUpdatesPerFrame = 5;
                    int updates = 0;
                    while (accumulator >= fixedDeltaTime && updates < maxUpdatesPerFrame) {
                        timeStep.duration = fixedDeltaTime.count();
                        pipelineService->update(context, timeStep);

                        const auto viewStates = viewStateWrite.load();
                        viewService->updateViewsState(*viewStates);

                        accumulator -= fixedDeltaTime;
                        updates++;
                        if(auto* frame = profiling::getCurrentFrame(context.profiling))
                        {
                            frame->updatesCount = updates;
                        }
                    }

                    profiling::popTimeSlice(context.profiling, steady_clock::now());
                }

                /*auto viewStates = viewStateRead.exchange(viewStateWrite.load());
                viewStateWrite.store(viewStates);*/

                viewService = _registry.get<IViewService>();
                if(viewService)
                {
                    profiling::pushTimeSlice(context.profiling, "rendering", steady_clock::now());

                    float alpha = static_cast<float>(accumulator.count()) / static_cast<float>(fixedDeltaTime.count());
                    alpha = glm::clamp(alpha, 0.0f, 1.0f);

                    const auto viewStates = viewStateWrite.load();

                    if(auto* frame = profiling::getCurrentFrame(context.profiling))
                    {
                        frame->viewStates.push_back(*viewStates);
                        frame->alpha = alpha;
                    }

                    viewService->updateViews(*viewStates, alpha, context.profiling);

                    profiling::popTimeSlice(context.profiling, steady_clock::now());
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
}