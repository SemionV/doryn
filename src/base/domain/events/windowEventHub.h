#pragma once

#include "event.h"
#include "base/typeComponents.h"

namespace dory::domain::events
{
    struct CloseWindowEventData
    {
        const int windowId;

        explicit CloseWindowEventData(int windowId):
            windowId(windowId)
        {
        }
    };

    template<class TDataContext>
    class WindowEventHub: Uncopyable
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
    class WindowEventHubDispatcher: public WindowEventHub<TDataContext>
    {
    private:
        EventBuffer<TDataContext, CloseWindowEventData> closeWindowEventBuffer;

    public:
        void addCase(const CloseWindowEventData& closeWindowData)
        {
            closeWindowEventBuffer.addCase(closeWindowData);
        }

        void submit(TDataContext& context)
        {
            closeWindowEventBuffer.submitCases(this->onCloseWindowDispatcher(), context);
        }
    };
}