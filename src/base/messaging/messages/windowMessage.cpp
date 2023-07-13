#include "base/dependencies.h"
#include "windowMessage.h"

namespace dory
{
    WindowMessage::WindowMessage(MessageType messageType, std::shared_ptr<Window> window):
        DeviceMessage(messageType),
        window(window)
    {
        
    }
}