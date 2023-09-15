#pragma once

namespace dory::win32
{
    struct Window: public dory::Window
    {
        HWND hWnd;

        Window(int id, HWND hWnd):
            dory::Window(id),
            hWnd(hWnd)
        {        
        }
    };
}