#include <dory/core/triggers/timeFrameTrigger.h>

namespace dory::core::triggers
{
    using namespace resources;

    TimeFrameTrigger::TimeFrameTrigger(Registry& registry):
        DependencyResolver(registry)
    {}

    entities::NodeUpdateCounter TimeFrameTrigger::check(IdType nodeId, const generic::model::TimeSpan& timeStep, DataContext& context)
    {
        const auto fixedDeltaTime = std::chrono::nanoseconds { nanoseconds };
        entities::NodeUpdateCounter updateCounter { 0, fixedDeltaTime };
        timeAccumulator += timeStep;

        while(timeAccumulator >= fixedDeltaTime && updateCounter.count < maxUpdatesPerFrame)
        {
            updateCounter.count++;
            timeAccumulator -= fixedDeltaTime;
        }

        if(updateCounter.count == maxUpdatesPerFrame)
        {
            timeAccumulator = {}; // reset accumulator to 0 in case if timeStep is too big
        }

        return updateCounter;
    }
}
