#pragma once

#include "windowParameters.h"
#include "window.h"
#include "messageBuffer.h"

namespace dory::win32
{
        class DORY_API WindowController: public domain::Controller
    {
        private:
            std::shared_ptr<domain::events::WindowEventHubDispatcher> eventHub;
            std::shared_ptr<MessageBuffer> messageBuffer;
            std::shared_ptr<domain::RepositoryReader<Window>> windowRepository;

            static bool compareHandles(Window* window, HWND hWnd);

        public:
            WindowController(std::shared_ptr<domain::events::WindowEventHubDispatcher> eventHub,
                std::shared_ptr<MessageBuffer> messageBuffer,
                std::shared_ptr<domain::RepositoryReader<Window>> windowRepository):
                eventHub(eventHub),
                messageBuffer(messageBuffer),
                windowRepository(windowRepository)
            {
            }

            virtual bool initialize(domain::entity::IdType referenceId, domain::DataContext& context) override;
            virtual void stop(domain::entity::IdType referenceId, domain::DataContext& context) override;
            virtual void update(domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, domain::DataContext& context) override;

        protected:
            void pumpSystemMessages();
            void submitEvents(domain::DataContext& context);
    };
}

/*Window creation Example:
    auto windowsThread = std::make_shared<dory::IndividualProcessThread>();
    auto windowRespositoryWin32 = std::make_shared<dory::EntityRepository<dory::win32::Window>>();
    auto windowAccessorWin32 = std::make_shared<dory::RepositoryReader<dory::win32::Window>>(windowRespositoryWin32);
    auto messageBufferWin32 = std::make_shared<dory::win32::MessageBuffer>();
    auto win32WindowEventHub = std::make_shared<dory::WindowEventHubDispatcher>();
    auto windowControllerWin32 = std::make_shared<dory::win32::WindowControllerParallel>(windowsThread, win32WindowEventHub, messageBufferWin32, windowAccessorWin32);
    auto windowControllerWin32Node = pipelineNodeRepository->store(dory::domain::entity::PipelineNode(idFactory->generate(), 
        windowControllerWin32, 0, inpoutGroupNode.id));
    windowControllerWin32->initialize(windowControllerWin32Node.id, context);

    windowsThread->run();

    dory::win32::WindowParameters win32WindowParameters;
    auto hWnd = dory::win32::WindowFactory::createWindow(win32WindowParameters, messageBufferWin32.get(), windowsThread);
    auto win32Window = windowRespositoryWin32->store(dory::win32::Window(idFactory->generate(), hWnd));

    win32WindowEventHub->onCloseWindow() += [&](dory::DataContext& context, dory::CloseWindowEventData& eventData)
    {
        auto windowId = eventData.windowId;
        auto window = windowAccessorWin32->get(windowId);

        if(window)
        {
            auto hWnd = window->hWnd;

            context.isStop = true;
            std::cout << "Close window(id " << windowId << ")" << std::endl;

            dory::win32::WindowFactory::closeWindow(hWnd, windowsThread);
            windowRespositoryWin32->remove(window);
        }
    };
*/