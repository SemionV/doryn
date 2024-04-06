#pragma once

#include "base/typeComponents.h"
#include "base/domain/events/eventBuffer.h"

namespace dory::domain::events
{
    namespace io
    {
        struct FlushOutputBufferEventData
        {};

        struct PressEscapeEventData
        {};

        struct PressEnterEventData
        {};
    }

    template<class TDataContext, typename TInputEventData, typename TOutputEventData>
    class InputEventHub: Uncopyable
    {
    protected:
        EventDispatcher<TDataContext&, TInputEventData&> inputEvent;
        EventDispatcher<TDataContext&, TOutputEventData&> outputEvent;
        EventDispatcher<TDataContext&, io::FlushOutputBufferEventData> flushBufferEvent;

        EventDispatcher<TDataContext&, io::PressEscapeEventData> pressEscapeEvent;
        EventDispatcher<TDataContext&, io::PressEnterEventData> pressEnterEvent;

    public:
        Event<TDataContext&, TInputEventData&>& onInput()
        {
            return inputEvent;
        }

        Event<TDataContext&, TOutputEventData&>& onOutput()
        {
            return outputEvent;
        }

        Event<TDataContext&, io::FlushOutputBufferEventData>& onFlush()
        {
            return flushBufferEvent;
        }

        Event<TDataContext&, io::PressEscapeEventData>& onPressEscape()
        {
            return pressEscapeEvent;
        }
        Event<TDataContext&, io::PressEnterEventData>& onPressEnter()
        {
            return pressEnterEvent;
        }

    };

    template<class TDataContext, typename TInputEventData, typename TOutputEventData>
    class InputEventDispatcher: public InputEventHub<TDataContext, TInputEventData, TOutputEventData>
    {
    private:
        EventBuffer<TDataContext, TInputEventData> inputEventBuffer;
        EventBuffer<TDataContext, TOutputEventData> outputEventBuffer;

        EventBuffer<TDataContext, io::PressEscapeEventData> pressEscaspeEventBuffer;
        EventBuffer<TDataContext, io::PressEnterEventData> pressEnterEventBuffer;

    public:
        void addCase(const TInputEventData& inputEventData)
        {
            inputEventBuffer.addCase(inputEventData);
        }

        void addCase(const TOutputEventData& outputEventData)
        {
            outputEventBuffer.addCase(outputEventData);
        }

        void addCase(const io::PressEscapeEventData eventData)
        {
            pressEscaspeEventBuffer.addCase(eventData);
        }

        void addCase(const io::PressEnterEventData eventData)
        {
            pressEnterEventBuffer.addCase(eventData);
        }

        void submitInput(TDataContext& context)
        {
            inputEventBuffer.submitCases(this->inputEvent, context);

            pressEscaspeEventBuffer.submitCases(this->pressEscapeEvent, context);
            pressEnterEventBuffer.submitCases(this->pressEnterEvent, context);
        }

        void submitOutput(TDataContext& context)
        {
            outputEventBuffer.submitCases(this->outputEvent, context);
        }

        void fire(TDataContext& context, const io::FlushOutputBufferEventData flushEventData)
        {
            this->flushBufferEvent(context, flushEventData);
        }
    };
}