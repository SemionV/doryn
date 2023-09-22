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
                std::shared_ptr<events::WindowEventHubDispatcher> eventHub,
                std::shared_ptr<MessageBuffer> messageBuffer,
                std::shared_ptr<RepositoryReader<Window>> windowRepository):
                WindowController(eventHub, messageBuffer, windowRepository),
                windowsThread(windowsThread)
            {
            }

            bool initialize(domain::entity::IdType referenceId, DataContext& context) override;
            void stop(domain::entity::IdType referenceId, DataContext& context) override;
            void update(domain::entity::IdType referenceId, const TimeSpan& timeStep, DataContext& context) override;
    };
}