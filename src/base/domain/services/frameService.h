#pragma once

#include "base/domain/types.h"

namespace dory::domain::services
{
    template<class TDataContext, typename TServiceLocator>
    class BasicFrameService: Service<TServiceLocator>
    {
    private:
        bool isStop = false;

    public:
        explicit BasicFrameService(TServiceLocator& serviceLocator):
            Service<TServiceLocator>(serviceLocator)
        {}

        void startLoop(TDataContext& context)
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

                this->services.engine.update(context, timeStep);

                lastTimestamp = currentTimestamp;
            }

            this->services.engine.stop(context);
        }

        void endLoop()
        {
            isStop = true;
        }
    };
}