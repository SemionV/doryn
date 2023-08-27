#pragma once

#include "base/dependencies.h"
#include "keyPressedEventData.h"
#include "base/messaging/event.h"
#include "base/messaging/eventHub.h"

namespace dory
{
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