#pragma once

#include "frameService.h"

namespace dory::domain::services
{
    template<class TDataContext>
    class BasicFrameService: public IFrameService<TDataContext>
    {
        private:
            bool isStop;

        public:
            void startLoop(domain::Engine<TDataContext>& engine, TDataContext& context) override
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

                    engine.update(context, timeStep);

                    lastTimestamp = currentTimestamp;
                }

                engine.stop(context);
            }

            void endLoop() override
            {
                isStop = true;
            }
    };
}