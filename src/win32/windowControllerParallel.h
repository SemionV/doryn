#pragma once

#include "windowController.h"

namespace dory::win32
{
    template<class TDataContext>
    class DORY_API WindowControllerParallel: public WindowController<TDataContext>
    {
        private:
            std::shared_ptr<multithreading::IndividualProcessThread> windowsThread;

        public:
            WindowControllerParallel(std::shared_ptr<multithreading::IndividualProcessThread> windowsThread,
                std::shared_ptr<domain::events::WindowEventHubDispatcher<TDataContext>> eventHub,
                std::shared_ptr<MessageBuffer> messageBuffer,
                std::shared_ptr<domain::RepositoryReader<Window>> windowRepository):
                WindowController<TDataContext>(eventHub, messageBuffer, windowRepository),
                windowsThread(windowsThread)
            {
            }

            bool initialize(domain::entity::IdType referenceId, TDataContext& context) override
            {
                auto pumpMessagesTask = multithreading::allocateActionTask([this]() 
                {
                    this->pumpSystemMessages();

                    const std::chrono::milliseconds threadMainSleepInterval = std::chrono::milliseconds(10);
                    std::this_thread::sleep_for(threadMainSleepInterval);
                });

                windowsThread->setRegularTask(pumpMessagesTask);

                return true;
            };

            void stop(domain::entity::IdType referenceId, TDataContext& context) override
            {

            };

            void update(domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, TDataContext& context) override
            {
                submitEvents(context);
            }
    };
}