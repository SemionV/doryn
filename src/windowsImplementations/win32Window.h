#pragma once

namespace doryWindows
{
    struct Win32Window: public dory::Window
    {
        const HWND hWnd;

        Win32Window(int id, HWND hWnd):
            dory::Window(id),
            hWnd(hWnd)
        {        
        }
    };
}