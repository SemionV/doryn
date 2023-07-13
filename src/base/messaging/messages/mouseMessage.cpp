#include "base/dependencies.h"
#include "mouseMessage.h"

namespace dory
{
    MouseMessage::MouseMessage(char buttonPressed):
        Message(MessageType::MouseTestMessage),
        buttonPressed(buttonPressed)
    {
    }
}