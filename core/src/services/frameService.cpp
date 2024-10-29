#include <dory/core/registry.h>
#include <dory/core/services/frameService.h>
#include <dory/core/services/iPipelineService.h>
#include <dory/generic/model.h>
#include <chrono>

namespace dory::core::services
{
    FrameService::FrameService(Registry& registry):
        _registry(registry)
    {}

    void FrameService::startLoop(resources::DataContext& context)
    {
        _registry.get<IPipelineService>([this, &context](IPipelineService* pipelineService){
            pipelineService->initialize(context);

            isStop = false;
            generic::model::TimeSpan timeStep(generic::model::UnitScale::Nano);

            std::chrono::steady_clock::time_point lastTimestamp = std::chrono::steady_clock::now();
            std::chrono::steady_clock::time_point currentTimestamp;
            std::chrono::nanoseconds duration;

            while(!isStop)
            {
                currentTimestamp = std::chrono::steady_clock::now();
                duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTimestamp - lastTimestamp);

                timeStep.duration = duration.count();

                pipelineService->update(context, timeStep);

                lastTimestamp = currentTimestamp;
            }

            pipelineService->stop(context);
        });
    }

    void FrameService::endLoop()
    {
        isStop = true;
    }
}