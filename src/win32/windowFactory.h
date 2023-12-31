#pragma once

#include "base/base.h"
#include "windowParameters.h"
#include "window.h"
#include "messageBuffer.h"

namespace dory::win32
{
    LRESULT WINAPI windowProcedure(HWND hWnd, UINT WindowsMessage, WPARAM wParam, LPARAM lParam);

    class DORY_API WindowFactory
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

            static HWND createWindow(WindowParameters windowParameters, MessageBuffer* messageBuffer, std::shared_ptr<concurrency::IndividualProcessThread> windowsThread)
            {
                auto createWindowTask = concurrency::allocateFunctionTask<HWND>([](WindowParameters windowParameters, MessageBuffer* messageBuffer)
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

            static void closeWindow(HWND hWnd, std::shared_ptr<concurrency::IndividualProcessThread> windowsThread)
            {
                auto createWindowTask = concurrency::allocateActionTask([](HWND hWnd)
                {
                    WindowFactory::closeWindow(hWnd);
                }, hWnd);

                windowsThread->invokeTask(createWindowTask);
            }
    };
}