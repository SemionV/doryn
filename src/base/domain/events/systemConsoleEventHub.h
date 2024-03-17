#pragma once

#include "base/dependencies.h"
#include "event.h"
#include "eventHub.h"
#include "base/typeComponents.h"

namespace dory::domain::events
{
    struct KeyPressedEventData
    {
        const char keyPressed;
        
        explicit KeyPressedEventData(char keyPressed):
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
        void addCase(KeyPressedEventData&& keyPressedData)
        {
            keyPressedEventBuffer.addCase(std::forward<KeyPressedEventData>(keyPressedData));
        }

        void submit(TDataContext& context) override
        {
            keyPressedEventBuffer.submitCases(this->onKeyPressedDispatcher(), context);
        }
    };
}