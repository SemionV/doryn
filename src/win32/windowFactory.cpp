#include "dependencies.h"
#include "windowFactory.h"

namespace dory::win32
{
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