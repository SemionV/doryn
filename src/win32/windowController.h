#pragma once

#include "windowParameters.h"
#include "window.h"
#include "messageBuffer.h"

namespace dory::win32
{
    template<class TDataContext>
    class WindowController: public domain::Controller<TDataContext>
    {
        private:
            std::shared_ptr<domain::events::WindowEventHubDispatcher<TDataContext>> eventHub;
            std::shared_ptr<MessageBuffer> messageBuffer;
            std::shared_ptr<domain::RepositoryReader<Window>> windowRepository;

            static bool compareHandles(Window* window, HWND hWnd)
            {
                return window->hWnd == hWnd;
            }

        public:
            WindowController(std::shared_ptr<domain::events::WindowEventHubDispatcher<TDataContext>> eventHub,
                std::shared_ptr<MessageBuffer> messageBuffer,
                std::shared_ptr<domain::RepositoryReader<Window>> windowRepository):
                eventHub(eventHub),
                messageBuffer(messageBuffer),
                windowRepository(windowRepository)
            {
            }

            virtual bool initialize(domain::entity::IdType referenceId, TDataContext& context) override
            {
                return true;
            };

            virtual void stop(domain::entity::IdType referenceId, TDataContext& context) override
            {
            };

            virtual void update(domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, TDataContext& context) override
            {
                pumpSystemMessages();     
                submitEvents(context);   
            }

        protected:
            void pumpSystemMessages()
            {
                MSG msg;

                while(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)) 
                {
                    if(GetMessage(&msg,NULL,0,0))
                    { 
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }		
                }

                std::size_t size = messageBuffer->messages.size();
                for(int i = 0; i < size; ++i)
                {
                    auto message = messageBuffer->messages[i];
                    if(message->messageId == WM_CLOSE)
                    {
                        std::cout << "Close win32 window" << std::endl;

                        auto window = windowRepository->get(message->hWnd, WindowController::compareHandles);
                        if(window)
                        {
                            eventHub->addCase(domain::events::CloseWindowEventData(window->id));
                        }
                    }
                }
                messageBuffer->reset();
            };

            void submitEvents(TDataContext& context)
            {
                eventHub->submit(context);
            };            
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