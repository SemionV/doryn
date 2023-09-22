#pragma once

#include "dependencies.h"
#include "windowParameters.h"
#include "window.h"
#include "messageBuffer.h"

namespace dory::win32
{
    LRESULT WINAPI windowProcedure(HWND hWnd, UINT WindowsMessage, WPARAM wParam, LPARAM lParam);

    class WindowFactory
    {
        public:
            static HWND createWindow(WindowParameters windowParameters, MessageBuffer* messageBuffer)
            {
                std::cout << std::this_thread::get_id() << ": create a system window" << std::endl;

                HINSTANCE hInstance = GetModuleHandle(NULL);

                WNDCLASS wc = { };

                auto windowClassName = _T("Sample Window Class");

                wc.lpfnWndProc   = windowProcedure;
                wc.hInstance     = hInstance;
                wc.lpszClassName = windowClassName;
                wc.cbWndExtra = sizeof(LONG_PTR);

                RegisterClass(&wc); 

                // Create the window.

                HWND hwnd = CreateWindowEx(
                    0,                              // Optional window styles.
                    windowClassName,                     // Window class
                    _T("dory"),    // Window text
                    WS_OVERLAPPEDWINDOW,            // Window style

                    // Size and position
                    CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,

                    NULL,       // Parent window    
                    NULL,       // Menu
                    hInstance,  // Instance handle
                    NULL        // Additional application data
                    );

                if (hwnd == NULL)
                {
                    return nullptr;
                }

                SetWindowLongPtr(hwnd, 0, (LONG_PTR)messageBuffer);

                ShowWindow(hwnd, SW_NORMAL);

                return hwnd;
            }

            static HWND createWindow(WindowParameters windowParameters, MessageBuffer* messageBuffer, std::shared_ptr<multithreading::IndividualProcessThread> windowsThread)
            {
                auto createWindowTask = multithreading::allocateFunctionTask<HWND>([](WindowParameters windowParameters, MessageBuffer* messageBuffer) 
                {
                    return WindowFactory::createWindow(windowParameters, messageBuffer);
                }, windowParameters, messageBuffer);

                windowsThread->invokeTask(createWindowTask);

                return createWindowTask->getResult();
            }

            static void closeWindow(HWND hWnd)
            {
                DestroyWindow(hWnd);
            }

            static void closeWindow(HWND hWnd, std::shared_ptr<multithreading::IndividualProcessThread> windowsThread)
            {
                auto createWindowTask = multithreading::allocateActionTask([](HWND hWnd) 
                {
                    WindowFactory::closeWindow(hWnd);
                }, hWnd);

                windowsThread->invokeTask(createWindowTask);
            }
    };

    LRESULT WINAPI windowProcedure(HWND hWnd, UINT WindowsMessage, WPARAM wParam, LPARAM lParam)
    {
        switch( WindowsMessage )
        {
            case WM_CLOSE:
            {
                MessageBuffer* messageBuffer = (MessageBuffer*)GetWindowLongPtr(hWnd, 0);
                if(messageBuffer)
                {
                    messageBuffer->onClose(WM_CLOSE, hWnd);
                }
                break;
            }
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                break;
            }

            case WM_TIMER:
            {
                break;
            }

            case WM_LBUTTONDOWN:
            {
                auto x = (int)LOWORD(GetMessagePos());
                auto y = (int)HIWORD(GetMessagePos());
                MessageBuffer* messageBuffer = (MessageBuffer*)GetWindowLongPtr(hWnd, 0);
                if(messageBuffer)
                {
                    messageBuffer->onClick(WM_LBUTTONDOWN, hWnd, x, y);
                }
                break;
            }

            case WM_LBUTTONUP:
            {
                break;
            }

            case WM_RBUTTONDOWN:
            {
                break;
            }

            case WM_RBUTTONUP:
            {	
                break;
            }

            case WM_MOUSEMOVE:
            {		
                break;
            }

            case WM_KEYDOWN:
            {
                switch(wParam)
                {

                case VK_F1:
                {			
                }

                };

                break;
            }

            case WM_KEYUP:
            {
                break;
            }

            case WM_CHAR:
            {			
            
                break;
            }

            default: 
            {
                return DefWindowProc(hWnd,WindowsMessage,wParam,lParam);
            }

        };

        return(0);	
    }
}