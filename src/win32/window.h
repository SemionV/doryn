#pragma once

namespace dory::win32
{
    struct Window: public dory::Window
    {
        HWND hWnd;

        Window(HWND hWnd):
            hWnd(hWnd)
        {}
    };
}