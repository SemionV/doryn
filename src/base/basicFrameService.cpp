#include "dependencies.h"
#include "basicFrameService.h"

namespace dory
{
    void BasicFrameService::startLoop(std::shared_ptr<Engine> engine)
    {
        isStop = false;
        TimeSpan timeStep(UnitScale::Nano);

        std::chrono::steady_clock::time_point lastTimestamp = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point currentTimestamp;
        std::chrono::nanoseconds duration;

        while(!isStop)
        {
            currentTimestamp = std::chrono::steady_clock::now();
            duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTimestamp - lastTimestamp);

            timeStep.duration = duration.count();

            isStop = engine->update(timeStep);

            lastTimestamp = currentTimestamp;
        }
    }
    
    void BasicFrameService::endLoop()
    {
        isStop = true;
    }
}