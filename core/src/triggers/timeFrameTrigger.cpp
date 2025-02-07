#include <dory/core/triggers/timeFrameTrigger.h>

namespace dory::core::triggers
{
    using namespace resources;

    TimeFrameTrigger::TimeFrameTrigger(Registry& registry):
        DependencyResolver(registry)
    {}

    entities::NodeUpdateCounter TimeFrameTrigger::check(IdType nodeId, const generic::model::TimeSpan& timeStep, DataContext& context)
    {
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

    bool TimeFrameTrigger::initialize(resources::IdType nodeId, resources::DataContext& context)
    {
        if(nanoseconds)
        {
            const std::chrono::duration<float, std::milli> duration { *nanoseconds };
            fixedDeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        }
        else if(microseconds)
        {
            const std::chrono::duration<float, std::micro> duration { *microseconds };
            fixedDeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        }
        else if(milliseconds)
        {
            const std::chrono::duration<float, std::milli> duration { *milliseconds };
            fixedDeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        }
        else if(seconds)
        {
            const std::chrono::duration<float> duration{ *seconds };
            fixedDeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        }
        else if(minutes)
        {
            const std::chrono::duration<float, std::ratio<60>> duration{ *minutes };
            fixedDeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        }

        return true;
    }
}
