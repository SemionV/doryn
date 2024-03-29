#pragma once

#include "dependencies.h"

namespace dory::win32
{
    struct Message
    {
        const UINT messageId;
        const HWND hWnd;

        Message(HWND hWnd, UINT messageId):
            hWnd(hWnd),
            messageId(messageId)
        {}
    };

    struct MessageClick: public Message
    {
        const int x;
        const int y;

        MessageClick(HWND hWnd, UINT messageId, int x, int y):
            Message(hWnd, messageId),
            x(x),
            y(y)
        {}
    };

    class MessageBuffer
    {
        public:
            std::vector<std::shared_ptr<Message>> messages;

            void onClick(UINT messageId, HWND hWnd, int x, int y)
            {
                auto message = std::make_shared<MessageClick>(hWnd, messageId, x, y);                
                messages.emplace_back(std::static_pointer_cast<Message>(message));
            }

            void onClose(UINT messageId, HWND hWnd)
            {
                auto message = std::make_shared<Message>(hWnd, messageId);    
                messages.emplace_back(message);
            }

            void reset()
            {
                messages.clear();
            }
    };
}