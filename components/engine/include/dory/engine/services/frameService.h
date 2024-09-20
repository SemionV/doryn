#pragma once

#include "dory/engine/resources/types.h"
#include "dory/engine/controllers/mainController.h"

namespace dory::engine::services
{
    template<typename TImplementation>
    class IFrameService: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename TDataContext, typename TEngine>
        void startLoop(TDataContext& context, controllers::IMainController<TEngine>& engine)
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
        void startLoopImpl(TDataContext& context, controllers::IMainController<TEngine>& engine)
        {
            engine.initialize(context);

            isStop = false;
            resources::TimeSpan timeStep(resources::UnitScale::Nano);

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