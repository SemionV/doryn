#pragma once

#include "base/dependencies.h"
#include "event.h"
#include "eventBuffer.h"
#include "base/typeComponents.h"

namespace dory::domain::events
{
    struct KeyPressedEventData
    {
        const int keyPressed;
        
        explicit KeyPressedEventData(int keyPressed):
            keyPressed(keyPressed)
        {
        }
    };

    template<class TDataContext>
    class SystemConsoleEventHub: Uncopyable
    {
    private:
        EventDispatcher<TDataContext&, KeyPressedEventData&> keyPressedEvent;

    protected:
        EventDispatcher<TDataContext&, KeyPressedEventData&>& onKeyPressedDispatcher()
        {
            return keyPressedEvent;
        }

    public:
        Event<TDataContext&, KeyPressedEventData&>& onKeyPressed()
        {
            return keyPressedEvent;
        }
    };

    template<class TDataContext>
    class SystemConsoleEventHubDispatcher: public EventHubDispatcher<TDataContext>, public SystemConsoleEventHub<TDataContext>
    {
    private:
        EventBuffer<TDataContext, KeyPressedEventData> keyPressedEventBuffer;

    public:
        void addCase(const KeyPressedEventData& keyPressedData)
        {
            keyPressedEventBuffer.addCase(keyPressedData);
        }

        void submit(TDataContext& context)
        {
            keyPressedEventBuffer.submitCases(this->onKeyPressedDispatcher(), context);
        }
    };
}