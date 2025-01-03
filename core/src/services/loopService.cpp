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

            std::size_t fps = 0;

            const auto fixedDeltaNanos = nanoseconds{16666667};
            auto accumulator = nanoseconds{0};
            auto fpsAccumulator = nanoseconds{0};
            auto fpsInterval = seconds{1};
            generic::model::TimeSpan timeStep(generic::model::UnitScale::Nano);

            steady_clock::time_point lastTimestamp = steady_clock::now();
            steady_clock::time_point currentTimestamp;

            SceneViewStateSet sceneStatesA;
            SceneViewStateSet sceneStatesB;

            std::atomic<SceneViewStateSet*> viewStateWrite { &sceneStatesA };
            /*std::atomic<SceneViewStateSet*> viewStateRead { &sceneStatesB };*/

            while(!isStop)
            {
                currentTimestamp = steady_clock::now();
                auto frameTime = duration_cast<nanoseconds>(currentTimestamp - lastTimestamp);
                lastTimestamp = currentTimestamp;

                if (frameTime > milliseconds(250)) {
                    frameTime = milliseconds(250);
                }

                accumulator += frameTime;
                fpsAccumulator += frameTime;

                pipelineService = _registry.get<IPipelineService>();
                auto viewService = _registry.get<IViewService>();
                if(pipelineService && viewService) //it can be hot-swapped, this is why reload it each frame
                {
                    while (accumulator >= fixedDeltaNanos) {
                        timeStep.duration = fixedDeltaNanos.count();
                        pipelineService->update(context, timeStep);

                        auto viewStates = viewStateWrite.load();
                        viewService->updateViewsState(*viewStates);

                        accumulator -= fixedDeltaNanos;
                    }
                }

                /*auto viewStates = viewStateRead.exchange(viewStateWrite.load());
                viewStateWrite.store(viewStates);*/

                viewService = _registry.get<IViewService>();
                if(viewService)
                {
                    float alpha = (float)accumulator.count() / (float)fixedDeltaNanos.count();
                    alpha = glm::clamp(alpha, 0.0f, 1.0f);

                    auto viewStates = viewStateWrite.load();
                    viewService->updateViews(*viewStates, alpha);
                }

                ++fps;

                if(fpsAccumulator >= fpsInterval)
                {
                    //TODO: output FPS
                    //logger->information(fmt::format("FPS: {0}", fps));
                    fps = 0;
                    fpsAccumulator = nanoseconds{0};
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
}