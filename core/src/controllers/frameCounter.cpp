#include <dory/core/registry.h>
#include <dory/core/controllers/frameCounter.h>
#include <spdlog/fmt/fmt.h>

namespace dory::core::controllers
{
    FrameCounter::FrameCounter(core::Registry& registry):
        DependencyResolver(registry)
    {}

    bool FrameCounter::initialize(resources::IdType nodeId, resources::DataContext& context)
    {
        getDuration(interval, fixedInterval);
        return true;
    }

    void FrameCounter::stop(resources::IdType nodeId, resources::DataContext& context)
    {
    }

    void FrameCounter::update(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context)
    {
        timeAccumulator += timeStep;
        while (timeAccumulator >= fixedInterval)
        {
            timeAccumulator = timeAccumulator - fixedInterval;

            //TODO: instead of the hardcoded action an event with a signature(for instance, name of the counter) should be fired
            if(auto logger = _registry.get<services::ILogService, resources::Logger::App>())
            {
                logger->information(fmt::format("FPS: {}", frameCount));
                if(auto* capture = getCurrentCapture(context.profiling))
                {
                    capture->done = true;
                }
            }
            frameCount = 0;
        }

        frameCount++;
    }
}
