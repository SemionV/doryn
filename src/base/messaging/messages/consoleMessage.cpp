#include "base/dependencies.h"
#include "consoleMessage.h"

namespace dory
{
    ConsoleMessage::ConsoleMessage(char keyPressed, int clickX, int clickY):
        keyPressed(keyPressed),
        clickX(clickX),
        clickY(clickY)
    {
    }
}