#pragma once

#include "event.h"

namespace dory::domain::events
{
    namespace io
    {
        enum class KeyCode
        {
            Unknown,
            Escape,
            Return,
            Backspace,
            Terminate,
            Character
        };

        struct KeyPressEvent
        {
            KeyCode keyCode = KeyCode::Unknown;
            int character = 0;
        };
    }

    template<class TDataContext>
    class InputEventHub: Uncopyable
    {
    protected:
        EventDispatcher<TDataContext&, io::KeyPressEvent> keyPressEvent;

    public:
        Event<TDataContext&, io::KeyPressEvent>& onKeyPress()
        {
            return keyPressEvent;
        }
    };

    template<class TDataContext>
    class InputEventDispatcher: public InputEventHub<TDataContext>
    {
    private:
        EventBuffer<TDataContext, io::KeyPressEvent> keyPressEventBuffer;

    public:
        void addCase(const TDataContext& context, const io::KeyPressEvent eventData)
        {
            keyPressEventBuffer.addCase(eventData);
        }

        void submit(TDataContext& context)
        {
            keyPressEventBuffer.submitCases(this->keyPressEvent, context);
        }
    };
}