#pragma once

namespace dory::win32
{
    class DORY_API ConsoleController: public dory::Controller
    {
        private:
            dory::IndividualProcessThread processThread;
            std::shared_ptr<events::SystemConsoleEventHubDispatcher> eventHub;

        public:
            ConsoleController(std::shared_ptr<events::SystemConsoleEventHubDispatcher> eventHub):
                eventHub(eventHub)
            {
            }

            bool initialize(domain::entity::IdType referenceId, DataContext& context) override;
            void stop(domain::entity::IdType referenceId, DataContext& context) override;
            void update(dory::domain::entity::IdType referenceId, const TimeSpan& timeStep, DataContext& context) override;

        protected:
            virtual void onKeyPressed(int key);
        
        private:
            void bindStdHandlesToConsole();
    };
}