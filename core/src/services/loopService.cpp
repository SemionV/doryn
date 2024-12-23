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
        _registry.get<IPipelineService>([this, &context](IPipelineService* pipelineService){
            pipelineService->initialize(context);

            isStop = false;

            const auto fixedDeltaNanos = std::chrono::nanoseconds{16666667}; //60 updates per second
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

                while (accumulator >= fixedDeltaNanos) {
                    timeStep.duration = fixedDeltaNanos.count();
                    pipelineService->update(context, timeStep);
                    accumulator -= fixedDeltaNanos;
                }
            }

            pipelineService->stop(context);
        });
    }

    void LoopService::endLoop()
    {
        isStop = true;
    }
}