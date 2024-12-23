#include <dory/core/registry.h>
#include <dory/core/services/loopService.h>
#include <dory/core/services/iPipelineService.h>
#include <dory/generic/model.h>
#include <chrono>

namespace dory::core::services
{
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

            const auto fixedDeltaNanos = std::chrono::nanoseconds{16666667};
            auto accumulator = std::chrono::nanoseconds{0};
            generic::model::TimeSpan timeStep(generic::model::UnitScale::Nano);

            std::chrono::steady_clock::time_point lastTimestamp = std::chrono::steady_clock::now();
            std::chrono::steady_clock::time_point currentTimestamp;
            std::chrono::nanoseconds duration;

            while(!isStop)
            {
                currentTimestamp = std::chrono::steady_clock::now();
                auto frameTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTimestamp - lastTimestamp);
                lastTimestamp = currentTimestamp;

                if (frameTime > std::chrono::milliseconds(250)) {
                    frameTime = std::chrono::milliseconds(250);
                }

                accumulator += frameTime;

                pipelineService = _registry.get<IPipelineService>();
                if(pipelineService) //it can be hot-swapped, this is why reload it each frame
                {
                    while (accumulator >= fixedDeltaNanos) {
                        timeStep.duration = fixedDeltaNanos.count();
                        pipelineService->update(context, timeStep);
                        accumulator -= fixedDeltaNanos;

                        //TODO: collect current write state for each view
                    }
                }

                float alpha = (float)accumulator.count() / (float)fixedDeltaNanos.count();
                auto viewRepo = _registry.get<repositories::IViewRepository>();
                if(viewRepo)
                {
                    viewRepo->each([alpha](const resources::entities::View& view) {
                        view.sceneStateWrite.load();
                    });
                }

                //TODO: swap read and write states for each view

                auto viewService = _registry.get<services::IViewService>();
                if(viewService)
                {
                    //TODO: draw current interpolated read state
                    viewService->updateViews(context);
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