#include "dependencies.h"
#include "win32WindowController.h"

namespace dory::win32
{
    bool Win32WindowController::initialize(DataContext& context)
    {
        return true;
    }

    void Win32WindowController::stop(DataContext& context)
    {
    }

    void Win32WindowController::update(const TimeSpan& timeStep, DataContext& context)
    {
        pumpSystemMessages();     
        submitEvents(context);   
    }

    void Win32WindowController::submitEvents(DataContext& context)
    {
        eventHub->submit(context);
    }

    void Win32WindowController::pumpSystemMessages()
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