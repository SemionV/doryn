#pragma once

namespace dory::win32
{
    struct Window: public domain::entity::Window
    {
        HWND hWnd;

        Window(dory::domain::entity::IdType id, HWND hWnd):
            domain::entity::Window(id),
            hWnd(hWnd)
        {}
    };
}