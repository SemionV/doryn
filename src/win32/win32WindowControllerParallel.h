#pragma once

#include "win32WindowController.h"

namespace dory::win32
{
    class DORY_API Win32WindowControllerParallel: public Win32WindowController
    {
        private:
            std::shared_ptr<dory::IndividualProcessThread> windowsThread;

        public:
            Win32WindowControllerParallel(std::shared_ptr<dory::IndividualProcessThread> windowsThread,
                std::shared_ptr<dory::WindowEventHubDispatcher> eventHub,
                std::shared_ptr<Win32MessageBuffer> messageBuffer,
                std::shared_ptr<Win32WindowRespository> windowRepository):
                Win32WindowController(eventHub, messageBuffer, windowRepository),
                windowsThread(windowsThread)
            {
            }

            bool initialize(DataContext& context) override;
            void stop(DataContext& context) override;
            void update(const TimeSpan& timeStep, DataContext& context) override;
    };
}