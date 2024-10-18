#pragma once

#include <termios.h>
#include <thread>
#include <iostream>
#include <memory>

#include <dory/core/devices/iDevice.h>
#include <dory/core/devices/iStandardOutputDevice.h>
#include <dory/core/events/eventTypes.h>

namespace dory::core::devices
{
    static struct termios oldt, currentt;

    class StandardIODevice: public IDevice, public IStandardOutputDevice
    {
    private:
        std::jthread pollingThread;
        bool connected = false;
        int pipes[2];

        std::shared_ptr<events::io::IEventDispatcher> _inputEventsDispatcher;

        void onKeyPressed(resources::DataContext& context, int key);

    public:
        explicit StandardIODevice(std::shared_ptr<events::io::IEventDispatcher> inputEventsDispatcher);
    };
}