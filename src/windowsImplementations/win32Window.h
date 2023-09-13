#pragma once

namespace doryWindows
{
    struct Win32Window: public dory::Window
    {
        const HWND hWnd;

        Win32Window(HWND hWnd):
            hWnd(hWnd)
        {        
        }
    };
}