#include "base/dependencies.h"
#include "consoleMessage.h"

namespace dory
{
    ConsoleMessage::ConsoleMessage(char keyPressed):
        DeviceMessage(MessageType::ConsoleTestMessage),
        keyPressed(keyPressed)
    {
    }
}