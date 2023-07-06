#include "base/dependencies.h"
#include "messagePool.h"

namespace dory
{
    MessagePool::MessagePool():
        messages()
    {
    }

    void MessagePool::addMessage(ConsoleMessage& message)
    {
        messages.push_back(message);
    }

    void MessagePool::clean()
    {
        messages.clear();
    }

    void MessagePool::iterate(std::function<void(ConsoleMessage)> lambda)
    {
        size_t count = messages.size();

        for(int i = 0; i < count; i++)
        {
            lambda(messages[i]);
        }
    }
}