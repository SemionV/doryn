#include <dory/core/registry.h>
#include <dory/core/services/loopService.h>
#include <dory/core/services/iPipelineService.h>
#include <chrono>
#include <dory/profiling/profiler.h>

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

        isStop = false;
        high_resolution_clock::time_point lastTimestamp = high_resolution_clock::now();

        static const char* loopName = "MainLoop";

        while(!isStop)
        {
            DORY_TRACE_FRAME_START(loopName);
            profiling::pushTimeSlice(context.profiling, std::string{ profiling::Profiling::frameRootTimeSlice });

            auto currentTimestamp = high_resolution_clock::now();
            auto frameTime = duration_cast<nanoseconds>(currentTimestamp - lastTimestamp);
            lastTimestamp = currentTimestamp;

            //Extra long frame time restriction(can be because of using debugger, etc.)
            if (frameTime > milliseconds(250)) {
                frameTime = milliseconds(250);
            }

            if(auto pipeline = _registry.get<IPipelineService>())
            {
                profiling::pushTimeSlice(context.profiling, "update");
                pipeline->update(context, frameTime);
                profiling::popTimeSlice(context.profiling);
            }

            profiling::popTimeSlice(context.profiling);
            DORY_TRACE_FRAME_END(loopName);
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