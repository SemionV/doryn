#pragma once

#include "windowController.h"

namespace dory::win32
{
    class DORY_API WindowControllerParallel: public WindowController
    {
        private:
            std::shared_ptr<dory::IndividualProcessThread> windowsThread;

        public:
            WindowControllerParallel(std::shared_ptr<dory::IndividualProcessThread> windowsThread,
                std::shared_ptr<dory::WindowEventHubDispatcher> eventHub,
                std::shared_ptr<MessageBuffer> messageBuffer,
                std::shared_ptr<EntityAccessor<Window>> windowRepository):
                WindowController(eventHub, messageBuffer, windowRepository),
                windowsThread(windowsThread)
            {
            }

            bool initialize(DataContext& context) override;
            void stop(DataContext& context) override;
            void update(const TimeSpan& timeStep, DataContext& context) override;
    };
}