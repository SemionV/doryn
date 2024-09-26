#pragma once

#include "dataContext.h"
#include <dory/engineObjects/events.h>

#ifdef __unix__
#include <dory/engine/devices/standartIoDeviceUnix.h>
#endif
#if (defined(_WIN32) || defined(_WIN64) || defined(WIN32))
#include <dory/engine/devices/standartIoDeviceWin32.h>
#endif

#include <dory/engine/devices/terminalDevice.h>

namespace dory::engine::devices
{
    extern template class ConsoleIODevice<DataContextType>;
    using StandartIODeviceType = ConsoleIODevice<DataContextType>;

    extern template class TerminalDevice<DataContextType, StandartIODeviceType>;
    using TerminalDeviceType = TerminalDevice<DataContextType, StandartIODeviceType>;

    struct DeviceLayer
    {
        StandartIODeviceType standardIoDevice;
        TerminalDeviceType terminalDevice;

        explicit DeviceLayer(events::EventLayer& events):
                standardIoDevice(events.standardIoDispatcher),
                terminalDevice{ standardIoDevice, events.standardInput, events.scriptDispatcher, events.applicationDispatcher }
        {}
    };
}