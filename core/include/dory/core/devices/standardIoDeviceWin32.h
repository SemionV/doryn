#pragma once

#include <thread>
#include <windows.h>
#include <wincon.h>
#include <winnt.h>
#include <fileapi.h>
#include <winbase.h>
#include <iostream>
#include <memory>

#include <dory/core/devices/iDevice.h>
#include <dory/core/devices/iStandardIODevice.h>
#include <dory/core/events/eventTypes.h>

namespace dory::core::devices
{
    class StandardIODevice: public IStandardIODevice
    {
    private:
        std::jthread pollingThread;
        bool connected = false;

        std::shared_ptr<events::io::IEventDispatcher> _inputEventsDispatcher;

        void bindStdHandlesToConsole();
        void onKeyPressed(resources::DataContext& context, INPUT_RECORD inputRecord);

    public:
        explicit StandardIODevice(std::shared_ptr<events::io::IEventDispatcher> inputEventsDispatcher);

        void out(const std::string& data) final;
        void flush() final;
        void connect(resources::DataContext& context) final;
        void disconnect(resources::DataContext& context) final;
    };
}