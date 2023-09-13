#pragma once

#include "openGL/dependencies.h"
#include "closeWindowEventData.h"

namespace doryOpenGL
{
    class GlfwWindowEventHub
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

    class GlfwWindowEventHubDispatcher: public dory::EventHubDispatcher, public GlfwWindowEventHub
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