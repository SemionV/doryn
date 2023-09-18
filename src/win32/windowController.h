#pragma once

#include "windowParameters.h"
#include "window.h"
#include "messageBuffer.h"

namespace dory::win32
{
        class DORY_API WindowController: public Controller
    {
        private:
            std::shared_ptr<dory::WindowEventHubDispatcher> eventHub;
            std::shared_ptr<MessageBuffer> messageBuffer;
            std::shared_ptr<RepositoryReader<Window>> windowRepository;

            static bool compareHandles(Window& window, HWND hWnd);

        public:
            WindowController(std::shared_ptr<dory::WindowEventHubDispatcher> eventHub,
                std::shared_ptr<MessageBuffer> messageBuffer,
                std::shared_ptr<RepositoryReader<Window>> windowRepository):
                eventHub(eventHub),
                messageBuffer(messageBuffer),
                windowRepository(windowRepository)
            {
            }

            virtual bool initialize(DataContext& context) override;
            virtual void stop(DataContext& context) override;
            virtual void update(const int referenceId, const TimeSpan& timeStep, DataContext& context) override;

        protected:
            void pumpSystemMessages();
            void submitEvents(DataContext& context);
    };
}