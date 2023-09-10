#pragma once

#include "base/dependencies.h"
#include "clickEventData.h"
#include "createWindowEventData.h"
#include "base/messaging/event.h"
#include "base/messaging/eventHub.h"

namespace dory
{
    class SystemWindowEventHub
    {
        private:
            EventDispatcher<DataContext&, MouseClickEventData&> mouseClickEvent;
            EventDispatcher<DataContext&, CreateWindowEventData&> createWindowEvent;

        protected:
            EventDispatcher<DataContext&, MouseClickEventData&>& onMouseClickDispatcher()
            {
                return mouseClickEvent;
            }

            EventDispatcher<DataContext&, CreateWindowEventData&>& onCreateWindowDispatcher()
            {
                return createWindowEvent;
            }

        public:
            Event<DataContext&, MouseClickEventData&>& onMouseClick()
            {
                return mouseClickEvent;
            }

            Event<DataContext&, CreateWindowEventData&>& onCreateWindowClick()
            {
                return createWindowEvent;
            }
    };

    class SystemWindowEventHubDispatcher: public EventHubDispatcher, public SystemWindowEventHub
    {
        private:
            EventBuffer<MouseClickEventData> mouseClickEventBuffer;
            EventBuffer<CreateWindowEventData> createWindowEventBuffer;

        public:
            void addCase(MouseClickEventData&& mouseClickData)
            {
                mouseClickEventBuffer.addCase(std::forward<MouseClickEventData>(mouseClickData));
            }

            void addCase(CreateWindowEventData&& createWindowData)
            {
                createWindowEventBuffer.addCase(std::forward<CreateWindowEventData>(createWindowData));
            }

            void submit(DataContext& context) override
            {
                mouseClickEventBuffer.submitCases(onMouseClickDispatcher(), context);
                createWindowEventBuffer.submitCases(onCreateWindowDispatcher(), context);
            }
    };
}