#pragma once

namespace dory::win32
{
    struct Window: public domain::entity::Window
    {
        HWND hWnd;

        Window(HWND hWnd):
            hWnd(hWnd)
        {}
    };
}