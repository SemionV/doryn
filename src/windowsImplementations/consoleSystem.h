#pragma once

namespace doryWindows
{
    class DORY_API ConsoleSystem: public dory::ConsoleSystem
    {
        private:
            dory::IndividualProcessThread processThread;
            std::shared_ptr<dory::SystemConsoleEventHubDispatcher> eventHub;

        public:
            ConsoleSystem(std::shared_ptr<dory::SystemConsoleEventHubDispatcher> eventHub):
                eventHub(eventHub)
            {
            }

            virtual bool connect() override;
            virtual void disconnect() override;
            virtual void update() override;

        protected:
            virtual void onKeyPressed(int key);
        
        private:
            void bindStdHandlesToConsole();
    };
}