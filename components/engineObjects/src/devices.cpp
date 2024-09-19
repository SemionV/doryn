#include <dory/engineObjects/devices.h>

namespace dory::engine::devices
{
    template class ConsoleIODevice<DataContextType>;
    template class TerminalDevice<DataContextType, DeviceTypeRegistry::StandartIODeviceType>;
}