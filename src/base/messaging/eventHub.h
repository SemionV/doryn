#pragma once

#include "base/doryExport.h"
#include "event.h"
#include "base/devices/window.h"

namespace dory
{
    class EventHubDispatcher
    {
        public:
            virtual void submit() = 0;
    };

    struct WindowClick
    {
        public:
            const std::shared_ptr<Window> window;
            const int x;
            const int y;

        public:
            WindowClick(std::shared_ptr<Window> window, int x, int y):
                window(window),
                x(x),
                y(y)
            {
            }
    };
    

    class WindowEventHub
    {
        private:
            EventDispatcher<WindowClick&> windowClick;

        public:
            Event<WindowClick&>& getWindowClick()
            {
                return windowClick;
            }

        protected:
            EventDispatcher<WindowClick&>& getWindowClickDispatcher()
            {
                return windowClick;
            }
    };

    class WindowEventHubDispatcher: public WindowEventHub, public EventHubDispatcher
    {
        private:
            std::vector<WindowClick> windowClicks;

        public:
            void onWindowClick(WindowClick&& windowClick)
            {
                windowClicks.emplace_back(std::forward<WindowClick>(windowClick));
            }

            void submit() override;
    };
}