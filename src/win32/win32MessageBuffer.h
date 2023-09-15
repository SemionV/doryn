#pragma once

#include "dependencies.h"

namespace dory::win32
{
    struct Win32Message
    {
        const UINT messageId;
        const HWND hWnd;

        Win32Message(HWND hWnd, UINT messageId):
            hWnd(hWnd),
            messageId(messageId)
        {}
    };

    struct Win32MessageClick: public Win32Message
    {
        const int x;
        const int y;

        Win32MessageClick(HWND hWnd, UINT messageId, int x, int y):
            Win32Message(hWnd, messageId),
            x(x),
            y(y)
        {}
    };

    class Win32MessageBuffer
    {
        public:
            std::vector<std::shared_ptr<Win32Message>> messages;

            void onClick(UINT messageId, HWND hWnd, int x, int y)
            {
                auto message = std::make_shared<Win32MessageClick>(hWnd, messageId, x, y);                
                messages.emplace_back(std::static_pointer_cast<Win32Message>(message));
            }

            void onClose(UINT messageId, HWND hWnd)
            {
                auto message = std::make_shared<Win32Message>(hWnd, messageId);    
                messages.emplace_back(message);
            }

            void reset()
            {
                messages.clear();
            }
    };
}