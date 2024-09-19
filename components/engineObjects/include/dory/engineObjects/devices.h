#pragma once

#include <dory/engineObjects/resources/dataContext.h>

#ifdef __unix__
#include <dory/engine/devices/standartIoDeviceUnix.h>
#endif
#ifdef WIN32
#include <dory/engine/devices/standartIoDeviceWin32.h>
#endif

#include <dory/engine/devices/terminalDevice.h>

namespace dory::engine::devices
{
    struct DeviceTypeRegistry
    {
        using StandartIODeviceType = ConsoleIODevice<DataContextType>;
        using TerminalDeviceType = TerminalDevice<DataContextType, StandartIODeviceType>;
    };

    extern template class ConsoleIODevice<DataContextType>;
    extern template class TerminalDevice<DataContextType, DeviceTypeRegistry::StandartIODeviceType>;
}