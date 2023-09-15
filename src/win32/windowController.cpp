#include "dependencies.h"
#include "windowController.h"

namespace dory::win32
{
    bool WindowController::initialize(DataContext& context)
    {
        return true;
    }

    void WindowController::stop(DataContext& context)
    {
    }

    void WindowController::update(const TimeSpan& timeStep, DataContext& context)
    {
        pumpSystemMessages();     
        submitEvents(context);   
    }

    void WindowController::submitEvents(DataContext& context)
    {
        eventHub->submit(context);
    }

    void WindowController::pumpSystemMessages()
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

                auto window = windowRepository->getByHWND(message->hWnd);
                if(window)
                {
                    eventHub->addCase(CloseWindowEventData(window->id));
                }
            }
        }
        messageBuffer->reset();
    }
}