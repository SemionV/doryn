#pragma once

#include "win32WindowParameters.h"
#include "win32Window.h"
#include "win32MessageBuffer.h"
#include "win32WindowRepository.h"

namespace dory::win32
{
        class DORY_API Win32WindowController: public Controller
    {
        private:
            std::shared_ptr<dory::WindowEventHubDispatcher> eventHub;
            std::shared_ptr<Win32MessageBuffer> messageBuffer;
            std::shared_ptr<Win32WindowRespository> windowRepository;

        public:
            Win32WindowController(std::shared_ptr<dory::WindowEventHubDispatcher> eventHub,
                std::shared_ptr<Win32MessageBuffer> messageBuffer,
                std::shared_ptr<Win32WindowRespository> windowRepository):
                eventHub(eventHub),
                messageBuffer(messageBuffer),
                windowRepository(windowRepository)
            {
            }

            virtual bool initialize(DataContext& context) override;
            virtual void stop(DataContext& context) override;
            virtual void update(const TimeSpan& timeStep, DataContext& context) override;

        protected:
            void pumpSystemMessages();
            void submitEvents(DataContext& context);
    };
}