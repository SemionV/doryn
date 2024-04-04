#pragma once

#include "base/typeComponents.h"
#include "base/domain/events/eventHub.h"

namespace dory::domain::events
{
    template<typename T>
    struct IOEvent
    {
        T value;
    };

    template<class TDataContext, typename TInputEventData, typename TOutputEventData>
    class StandartInputOutputEventHub: Uncopyable
    {
    private:
        EventDispatcher<TDataContext&, const TInputEventData&> inputEvent;
        EventDispatcher<TDataContext&, const TOutputEventData&> outputEvent;

    protected:
        EventDispatcher<TDataContext&, const TInputEventData&>& onInputDispatcher()
        {
            return inputEvent;
        }

        EventDispatcher<TDataContext&, const TOutputEventData&>& onOutputDispatcher()
        {
            return outputEvent;
        }

    public:
        Event<TDataContext&, const TInputEventData&>& onInput()
        {
            return inputEvent;
        }

        Event<TDataContext&, const TOutputEventData&>& onOutput()
        {
            return outputEvent;
        }
    };

    template<class TDataContext, typename TInputEventData, typename TOutputEventData>
    class StandartInputOutputEventHubDispatcher: public StandartInputOutputEventHub<TDataContext, TInputEventData, TOutputEventData>
    {
    private:
        EventBuffer<TDataContext, TInputEventData> inputEventBuffer;
        EventBuffer<TDataContext, TOutputEventData> outputEventBuffer;

    public:
        void addCase(const TInputEventData& inputEventData)
        {
            inputEventBuffer.addCase(inputEventData);
        }

        void addCase(const TOutputEventData& outputEventData)
        {
            outputEventBuffer.addCase(outputEventData);
        }

        void submit(TDataContext& context)
        {
            inputEventBuffer.submitCases(this->onInputDispatcher(), context);
            outputEventBuffer.submitCases(this->onOutputDispatcher(), context);
        }
    };
}