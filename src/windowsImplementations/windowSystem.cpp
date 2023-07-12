#include "dependencies.h"
#include "windowSystem.h"

namespace doryWindows
{
    bool WindowSystem::connect()
    {
        return true;
    }

    void WindowSystem::disconnect()
    {
    }

    void WindowSystem::update()
    {
        pumpSystemMessages();
    }

    void WindowSystem::pumpSystemMessages()
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
    }

    std::shared_ptr<dory::Window> WindowSystem::createWindow(const WindowParameters& parameters)
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
            return nullptr;
        }

        SetWindowLongPtr(hwnd, 0, (LONG_PTR)this);

        ShowWindow(hwnd, SW_NORMAL);

        return std::make_shared<dory::Window>();
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
                /*SystemWindow* window = (SystemWindow*)GetWindowLongPtr(hWnd, 0);
                if(window)
                {
                    window->addClickMessage(pp.x, pp.y);
                }*/
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