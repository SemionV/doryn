#include "base/dependencies.h"
#include "systemWindow.h"

namespace dory
{
    int clickX(-1);
    int clickY(-1);

    SystemWindow::SystemWindow():
        isStop(false)
    {        
    }

    bool SystemWindow::connect()
    {
        isStop = false;

        std::cout << "SystemWindow.connect()" << std::endl;

        workingThread = std::thread(&monitorSystemWindow, this);
        workingThread.detach();

        return true;
    }

    void SystemWindow::monitorSystemWindow()
    {
        createWindow();

        MSG msg;

        while(GetMessage(&msg, NULL, 0, 0) != 0 && !isStop)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void SystemWindow::disconnect()
    {
        isStop = true;
    }
    
    void SystemWindow::readUpdates(MessagePool& messagePool)
    {
        if(clickX >= 0)
        {
            ConsoleMessage message(0, clickX, clickY);
            messagePool.addMessage(message);

            clickX = -1;
            clickY = -1;
        }
    }

    LRESULT WINAPI windowMessageHandler(HWND hWnd, UINT WindowsMessage, WPARAM wParam, LPARAM lParam)
    {
        switch( WindowsMessage )
        {
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;

                break;
            }

            case WM_TIMER:
            {
                break;
            }

            case WM_LBUTTONDOWN:
            {
                POINT pp;
                GetCursorPos(&pp);
                clickX = pp.x;
                clickY = pp.y;
                break;
            }

            case WM_LBUTTONUP:
            {
                break;
            }

            case WM_RBUTTONDOWN:
            {
                POINT pp;
                GetCursorPos(&pp);
                break;
            }

            case WM_RBUTTONUP:
            {	
                break;
            }

            case WM_MOUSEMOVE:
            {		
                POINT pp;
                GetCursorPos(&pp);

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

            default : return DefWindowProc(hWnd,WindowsMessage,wParam,lParam);

        };

        return(0);	
    }

    int SystemWindow::createWindow()
    {
        // Register the window class.
        //const char_t CLASS_NAME[]  = "Sample Window Class";

        HINSTANCE hInstance = GetModuleHandle(NULL);

        WNDCLASS wc = { };

        wc.lpfnWndProc   = windowMessageHandler;
        wc.hInstance     = hInstance;
        wc.lpszClassName = "Sample Window Class";

        RegisterClass(&wc);

        // Create the window.

        HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles.
            "Sample Window Class",                     // Window class
            "Learn to Program Windows",    // Window text
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
            return 0;
        }

        ShowWindow(hwnd, SW_NORMAL);

        return 1;
    }
}