#pragma once

#include "base/typeComponents.h"
#include "base/domain/events/eventBuffer.h"

namespace dory::domain::events
{
    struct FlushOutputBuffer
    {};

    template<class TDataContext, typename TInputEventData, typename TOutputEventData>
    class IOEventHub: Uncopyable
    {
    protected:
        EventDispatcher<TDataContext&, TInputEventData&> inputEvent;
        EventDispatcher<TDataContext&, TOutputEventData&> outputEvent;
        EventDispatcher<TDataContext&, FlushOutputBuffer> flushBufferEvent;

    public:
        Event<TDataContext&, TInputEventData&>& onInput()
        {
            return inputEvent;
        }

        Event<TDataContext&, TOutputEventData&>& onOutput()
        {
            return outputEvent;
        }

        Event<TDataContext&, FlushOutputBuffer>& onFlush()
        {
            return flushBufferEvent;
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
            inputEventBuffer.submitCases(this->inputEvent, context);
        }

        void submitOutput(TDataContext& context)
        {
            outputEventBuffer.submitCases(this->outputEvent, context);
        }

        void fire(TDataContext& context, const FlushOutputBuffer flushEventData)
        {
            this->flushBufferEvent(context, flushEventData);
        }
    };
}