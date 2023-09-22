#pragma once

namespace dory::win32
{
    class DORY_API ConsoleController: public domain::Controller
    {
        private:
            multithreading::IndividualProcessThread processThread;
            std::shared_ptr<domain::events::SystemConsoleEventHubDispatcher> eventHub;

        public:
            ConsoleController(std::shared_ptr<domain::events::SystemConsoleEventHubDispatcher> eventHub):
                eventHub(eventHub)
            {
            }

            bool initialize(domain::entity::IdType referenceId, domain::DataContext& context) override;
            void stop(domain::entity::IdType referenceId, domain::DataContext& context) override;
            void update(dory::domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, domain::DataContext& context) override;

        protected:
            virtual void onKeyPressed(int key);
        
        private:
            void bindStdHandlesToConsole();
    };
}