#pragma once

namespace dory::win32
{
    class DORY_API ConsoleController: public dory::Controller
    {
        private:
            dory::IndividualProcessThread processThread;
            std::shared_ptr<dory::SystemConsoleEventHubDispatcher> eventHub;

        public:
            ConsoleController(std::shared_ptr<dory::SystemConsoleEventHubDispatcher> eventHub):
                eventHub(eventHub)
            {
            }

            bool initialize(DataContext& context) override;
            void stop(DataContext& context) override;
            void update(const int referenceId, const TimeSpan& timeStep, DataContext& context) override;

        protected:
            virtual void onKeyPressed(int key);
        
        private:
            void bindStdHandlesToConsole();
    };
}