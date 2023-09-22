#pragma once

#include "event.h"

namespace dory::domain::events
{
    struct CloseWindowEventData
    {
        const int windowId;

        CloseWindowEventData(int windowId):
            windowId(windowId)
        {
        }
    };

    class WindowEventHub
    {
        private:
            EventDispatcher<DataContext&, CloseWindowEventData&> closeWindowEvent;

        protected:
            EventDispatcher<DataContext&, CloseWindowEventData&>& onCloseWindowDispatcher()
            {
                return closeWindowEvent;
            }

        public:
            Event<DataContext&, CloseWindowEventData&>& onCloseWindow()
            {
                return closeWindowEvent;
            }
    };

    class WindowEventHubDispatcher: public EventHubDispatcher, public WindowEventHub
    {
        private:
            EventBuffer<CloseWindowEventData> closeWindowEventBuffer;

        public:
            void addCase(CloseWindowEventData&& closeWindowData)
            {
                closeWindowEventBuffer.addCase(std::forward<CloseWindowEventData>(closeWindowData));
            }

            void submit(DataContext& context) override
            {
                closeWindowEventBuffer.submitCases(onCloseWindowDispatcher(), context);
            }
    };
}