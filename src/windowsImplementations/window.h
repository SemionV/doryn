#pragma once

namespace doryWindows
{
    struct Window: public dory::Window
    {
        const HWND hWnd;

        Window(HWND hWnd):
            hWnd(hWnd)
        {        
        }
    };
}