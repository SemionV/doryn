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

    template<class TDataContext>
    class WindowEventHub
    {
        private:
            EventDispatcher<TDataContext&, CloseWindowEventData&> closeWindowEvent;

        protected:
            EventDispatcher<TDataContext&, CloseWindowEventData&>& onCloseWindowDispatcher()
            {
                return closeWindowEvent;
            }

        public:
            Event<TDataContext&, CloseWindowEventData&>& onCloseWindow()
            {
                return closeWindowEvent;
            }
    };

    template<class TDataContext>
    class WindowEventHubDispatcher: public EventHubDispatcher<TDataContext>, public WindowEventHub<TDataContext>
    {
        private:
            EventBuffer<TDataContext, CloseWindowEventData> closeWindowEventBuffer;

        public:
            void addCase(CloseWindowEventData&& closeWindowData)
            {
                closeWindowEventBuffer.addCase(std::forward<CloseWindowEventData>(closeWindowData));
            }

            void submit(TDataContext& context) override
            {
                closeWindowEventBuffer.submitCases(this->onCloseWindowDispatcher(), context);
            }
    };
}