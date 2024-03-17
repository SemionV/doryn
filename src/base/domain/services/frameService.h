#pragma once

#include "base/domain/types.h"
#include "base/typeComponents.h"
#include "base/domain/engine.h"

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

    template<typename TImplementation, typename TDataContext>
    class IFrameService: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        void startLoop(TDataContext& context)
        {
            this->toImplementation()->startLoopImpl(context);
        }

        void endLoop()
        {
            this->toImplementation()->endLoopImpl();
        }
    };

    template<class TDataContext, typename TEngine>
    class BasicFrameService2: public IFrameService<BasicFrameService2<TDataContext, TEngine>, TDataContext>
    {
    private:
        bool isStop = false;

        using EngineType = IEngine<TEngine, TDataContext>;
        EngineType& engine;

    public:
        explicit BasicFrameService2(EngineType& engine):
                engine(engine)
        {}

        void startLoopImpl(TDataContext& context)
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

        void endLoopImpl()
        {
            isStop = true;
        }
    };
}