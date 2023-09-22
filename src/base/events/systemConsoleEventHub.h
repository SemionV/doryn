#pragma once

#include "base/dependencies.h"
#include "event.h"
#include "eventHub.h"

namespace dory::events
{
    struct KeyPressedEventData
    {
        const char keyPressed;
        
        KeyPressedEventData(char keyPressed):
            keyPressed(keyPressed)
        {
        }
    };

    class SystemConsoleEventHub
    {
        private:
            EventDispatcher<DataContext&, KeyPressedEventData&> keyPressedEvent;

        protected:
            EventDispatcher<DataContext&, KeyPressedEventData&>& onKeyPressedDispatcher()
            {
                return keyPressedEvent;
            }

        public:
            Event<DataContext&, KeyPressedEventData&>& onKeyPressed()
            {
                return keyPressedEvent;
            }
    };

    class SystemConsoleEventHubDispatcher: public EventHubDispatcher, public SystemConsoleEventHub
    {
        private:
            EventBuffer<KeyPressedEventData> keyPressedEventBuffer;

        public:
            void addCase(KeyPressedEventData&& keyPressedData)
            {
                keyPressedEventBuffer.addCase(std::forward<KeyPressedEventData>(keyPressedData));
            }

            void submit(DataContext& context) override
            {
                keyPressedEventBuffer.submitCases(onKeyPressedDispatcher(), context);
            }
    };
}