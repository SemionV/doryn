#pragma once

namespace doryWindows
{
    class Window: public dory::Window
    {
        public:
            const HWND hWnd;
        
        public:
            Window(HWND hWnd):
                hWnd(hWnd)
            {        
            }
    };
}