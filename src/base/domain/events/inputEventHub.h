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
        EventDispatcher<TDataContext&, io::PressEnterEventData> pressEnterEvent;
        EventDispatcher<TDataContext&, io::PressSymbolEventData> pressSymbolEvent;

    public:
        Event<TDataContext&, io::PressEscapeEventData>& onPressEscape()
        {
            return pressEscapeEvent;
        }

        Event<TDataContext&, io::PressEnterEventData>& onPressEnter()
        {
            return pressEnterEvent;
        }

        Event<TDataContext&, io::PressSymbolEventData>& onPressSymbol()
        {
            return pressSymbolEvent;
        }
    };

    template<class TDataContext>
    class InputEventDispatcher: public InputEventHub<TDataContext>
    {
    private:
        EventBuffer<TDataContext, io::PressEscapeEventData> pressEscaspeEventBuffer;
        EventBuffer<TDataContext, io::PressEnterEventData> pressEnterEventBuffer;
        EventBuffer<TDataContext, io::PressSymbolEventData> pressSymbolEventBuffer;

    public:
        void addCase(const TDataContext& context, const io::PressEscapeEventData eventData)
        {
            pressEscaspeEventBuffer.addCase(eventData);
        }

        void addCase(const TDataContext& context, const io::PressEnterEventData eventData)
        {
            pressEnterEventBuffer.addCase(eventData);
        }

        void addCase(const TDataContext& context, const io::PressSymbolEventData eventData)
        {
            pressSymbolEventBuffer.addCase(eventData);
        }

        void submit(TDataContext& context)
        {
            pressEscaspeEventBuffer.submitCases(this->pressEscapeEvent, context);
            pressEnterEventBuffer.submitCases(this->pressEnterEvent, context);
            pressSymbolEventBuffer.submitCases(this->pressSymbolEvent, context);
        }
    };
}