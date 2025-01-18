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

            auto fpsAccumulator = nanoseconds{0};
            const auto fpsInterval = seconds{1};
            std::size_t frameCounter = 1;

            steady_clock::time_point lastTimestamp = steady_clock::now();

            while(!isStop)
            {
                steady_clock::time_point currentTimestamp = steady_clock::now();
                auto frameTime = duration_cast<nanoseconds>(currentTimestamp - lastTimestamp);
                lastTimestamp = currentTimestamp;

                profiling::popTimeSlice(context.profiling, currentTimestamp);

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
                }

                fpsAccumulator += frameTime;
                while (fpsAccumulator >= fpsInterval)
                {
                    fpsAccumulator = fpsAccumulator - fpsInterval;
                }

                if(auto pipeline = _registry.get<IPipelineService>()) //it can be hot-swapped, this is why reload it each frame
                {
                    profiling::pushTimeSlice(context.profiling, "update", steady_clock::now());
                    pipeline->update(context, std::chrono::duration_cast<generic::model::TimeSpan>(frameTime));
                    profiling::popTimeSlice(context.profiling, steady_clock::now());
                }

                if(auto viewService = _registry.get<IViewService>())
                {
                    profiling::pushTimeSlice(context.profiling, "rendering", steady_clock::now());

                    steady_clock::time_point now = steady_clock::now();

                    float alpha = duration_cast<generic::model::TimeSpan>(now - lastTimestamp).count() / context.viewStatesUpdateTimeDelta.count();
                    alpha = glm::clamp(alpha, 0.0f, 1.0f);

                    if(auto* frame = profiling::getCurrentFrame(context.profiling))
                    {
                        frame->viewStates.push_back(context.viewStates);
                        frame->alpha = alpha;
                    }

                    viewService->updateViews(context.viewStates, alpha, context.profiling);

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