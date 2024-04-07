#pragma once

#include "base/typeComponents.h"
#include "base/domain/events/eventBuffer.h"

namespace dory::domain::events
{
    namespace io
    {
        struct PressEscapeEventData
        {};

        struct PressEnterEventData
        {};

        struct PressBackspaceEventData
        {};

        struct PressSymbolEventData
        {
            int symbol;
        };
    }

    template<class TDataContext>
    class InputEventHub: Uncopyable
    {
    protected:
        EventDispatcher<TDataContext&, io::PressEscapeEventData> pressEscapeEvent;
        EventDispatcher<TDataContext&, io::PressEnterEventData> pressReturnEvent;
        EventDispatcher<TDataContext&, io::PressBackspaceEventData> pressBackspaceEvent;
        EventDispatcher<TDataContext&, io::PressSymbolEventData> enterSymbolEvent;

    public:
        Event<TDataContext&, io::PressEscapeEventData>& onPressEscape()
        {
            return pressEscapeEvent;
        }

        Event<TDataContext&, io::PressEnterEventData>& onPressReturn()
        {
            return pressReturnEvent;
        }

        Event<TDataContext&, io::PressBackspaceEventData>& onPressBackspace()
        {
            return pressBackspaceEvent;
        }

        Event<TDataContext&, io::PressSymbolEventData>& onEnterSymbol()
        {
            return enterSymbolEvent;
        }
    };

    template<class TDataContext>
    class InputEventDispatcher: public InputEventHub<TDataContext>
    {
    private:
        EventBuffer<TDataContext, io::PressEscapeEventData> pressEscaspeEventBuffer;
        EventBuffer<TDataContext, io::PressEnterEventData> pressReturnEventBuffer;
        EventBuffer<TDataContext, io::PressBackspaceEventData> pressBackspaceEventBuffer;
        EventBuffer<TDataContext, io::PressSymbolEventData> enterSymbolEventBuffer;

    public:
        void addCase(const TDataContext& context, const io::PressEscapeEventData eventData)
        {
            pressEscaspeEventBuffer.addCase(eventData);
        }

        void addCase(const TDataContext& context, const io::PressEnterEventData eventData)
        {
            pressReturnEventBuffer.addCase(eventData);
        }

        void addCase(const TDataContext& context, const io::PressBackspaceEventData eventData)
        {
            pressBackspaceEventBuffer.addCase(eventData);
        }

        void addCase(const TDataContext& context, const io::PressSymbolEventData eventData)
        {
            enterSymbolEventBuffer.addCase(eventData);
        }

        void submit(TDataContext& context)
        {
            pressEscaspeEventBuffer.submitCases(this->pressEscapeEvent, context);
            pressReturnEventBuffer.submitCases(this->pressReturnEvent, context);
            pressBackspaceEventBuffer.submitCases(this->pressBackspaceEvent, context);
            enterSymbolEventBuffer.submitCases(this->enterSymbolEvent, context);
        }
    };
}