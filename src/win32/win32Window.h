#pragma once

namespace dory::win32
{
    struct Win32Window: public dory::Window
    {
        HWND hWnd;

        Win32Window(int id, HWND hWnd):
            dory::Window(id),
            hWnd(hWnd)
        {        
        }
    };
}