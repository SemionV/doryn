#pragma once

#include "base/typeComponents.h"
#include "base/domain/events/eventBuffer.h"

namespace dory::domain::events
{
    template<class TDataContext, typename TInputEventData, typename TOutputEventData>
    class IOEventHub: Uncopyable
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
    class IOEventHubDispatcher: public IOEventHub<TDataContext, TInputEventData, TOutputEventData>
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

        void submitInput(TDataContext& context)
        {
            inputEventBuffer.submitCases(this->onInputDispatcher(), context);
        }

        void submitOutput(TDataContext& context)
        {
            outputEventBuffer.submitCases(this->onOutputDispatcher(), context);
        }
    };
}