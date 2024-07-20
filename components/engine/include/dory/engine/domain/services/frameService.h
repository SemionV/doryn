#pragma once

#include <dory/engine/domain/types.h>
#include <dory/engine/domain/engine.h>

namespace dory::domain::services
{
    template<typename TImplementation>
    class IFrameService: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename TDataContext, typename TEngine>
        void startLoop(TDataContext& context, IEngine<TEngine, TDataContext>& engine)
        {
            this->toImplementation()->startLoopImpl(context, engine);
        }

        void endLoop()
        {
            this->toImplementation()->endLoopImpl();
        }
    };

    class BasicFrameService: public IFrameService<BasicFrameService>
    {
    private:
        bool isStop = false;

    public:

        template<typename TDataContext, typename TEngine>
        void startLoopImpl(TDataContext& context, IEngine<TEngine, TDataContext>& engine)
        {
            engine.initialize(context);

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

        void endLoopImpl()
        {
            isStop = true;
        }
    };
}