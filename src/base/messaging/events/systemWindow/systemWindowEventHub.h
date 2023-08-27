#pragma once

#include "base/dependencies.h"
#include "clickEventData.h"
#include "base/messaging/event.h"
#include "base/messaging/eventHub.h"

namespace dory
{
    class SystemWindowEventHub
    {
        private:
            EventDispatcher<DataContext&, MouseClickEventData&> mouseClickEvent;

        protected:
            EventDispatcher<DataContext&, MouseClickEventData&>& onMouseClickDispatcher()
            {
                return mouseClickEvent;
            }

        public:
            Event<DataContext&, MouseClickEventData&>& onMouseClick()
            {
                return mouseClickEvent;
            }
    };

    class SystemWindowEventHubDispatcher: public EventHubDispatcher, public SystemWindowEventHub
    {
        private:
            EventBuffer<MouseClickEventData> mouseClickEventBuffer;

        public:
            void addCase(MouseClickEventData&& mouseClickData)
            {
                mouseClickEventBuffer.addCase(std::forward<MouseClickEventData>(mouseClickData));
            }

            void submit(DataContext& context) override
            {
                mouseClickEventBuffer.submitCases(onMouseClickDispatcher(), context);
            }
    };
}