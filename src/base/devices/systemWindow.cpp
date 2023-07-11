#include "base/dependencies.h"
#include "systemWindow.h"

namespace dory
{
    LRESULT WINAPI windowProcedure(HWND hWnd, UINT WindowsMessage, WPARAM wParam, LPARAM lParam);

    SystemWindow::SystemWindow():
        clickX(-1),
        clickY(-1)
    {
        pumpMessagesTask = new LambdaTask([]() 
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

            const std::chrono::milliseconds threadMainSleepInterval = std::chrono::milliseconds(10);
            std::this_thread::sleep_for(threadMainSleepInterval);
        });

        systemThread = new SystemThread(pumpMessagesTask);
    }

    bool SystemWindow::connect()
    {
        std::cout << "SystemWindow.connect()" << std::endl;

        systemThread->run();

        auto createWindowTask = new LambdaTask([this]() 
        {
            std::cout << std::this_thread::get_id() << ": create a system window" << std::endl;
            this->createWindow();
        });

        systemThread->invokeTask(createWindowTask);

        return true;
    }

    void SystemWindow::disconnect()
    {
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

    void SystemWindow::addClickMessage(int clickX, int clickY)
    {
        std::cout << std::this_thread::get_id() << ": add click message (" << clickX << ", " << clickY << ")" << std::endl;
        const std::lock_guard<std::mutex> lock(mutex);
        this->clickX = clickX;
        this->clickY = clickY;
    }

    int SystemWindow::createWindow()
    {
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
            return 0;
        }

        SetWindowLongPtr(hwnd, 0, (LONG_PTR)this);

        ShowWindow(hwnd, SW_NORMAL);

        return 1;
    }

    LRESULT WINAPI windowProcedure(HWND hWnd, UINT WindowsMessage, WPARAM wParam, LPARAM lParam)
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
                SystemWindow* window = (SystemWindow*)GetWindowLongPtr(hWnd, 0);
                if(window)
                {
                    window->addClickMessage(pp.x, pp.y);
                }
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

            default: 
            {
                return DefWindowProc(hWnd,WindowsMessage,wParam,lParam);
            }

        };

        return(0);	
    }
}