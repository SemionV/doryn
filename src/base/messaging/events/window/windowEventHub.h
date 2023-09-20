#pragma once

namespace dory
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
            dory::EventDispatcher<dory::DataContext&, CloseWindowEventData&> closeWindowEvent;

        protected:
            dory::EventDispatcher<dory::DataContext&, CloseWindowEventData&>& onCloseWindowDispatcher()
            {
                return closeWindowEvent;
            }

        public:
            dory::Event<dory::DataContext&, CloseWindowEventData&>& onCloseWindow()
            {
                return closeWindowEvent;
            }
    };

    class WindowEventHubDispatcher: public dory::EventHubDispatcher, public WindowEventHub
    {
        private:
            dory::EventBuffer<CloseWindowEventData> closeWindowEventBuffer;

        public:
            void addCase(CloseWindowEventData&& closeWindowData)
            {
                closeWindowEventBuffer.addCase(std::forward<CloseWindowEventData>(closeWindowData));
            }

            void submit(dory::DataContext& context) override
            {
                closeWindowEventBuffer.submitCases(onCloseWindowDispatcher(), context);
            }
    };
}