#pragma once

#include <termios.h>
#include <thread>
#include <iostream>
#include <memory>

#include <dory/core/registry.h>
#include <dory/core/devices/iStandardIODevice.h>
#include <dory/core/events/eventTypes.h>

namespace dory::core::devices
{
    static struct termios oldt, currentt;

    class StandardIODevice: public IStandardIODevice
    {
    private:
        std::jthread pollingThread;
        bool connected = false;
        int pipes[2];

        Registry& _registry;

        void onKeyPressed(resources::DataContext& context, int key);

    public:
        explicit StandardIODevice(Registry& registry);

        void out(const std::string& data) final;
        void flush() final;
        void connect(resources::DataContext& context) final;
        void disconnect(resources::DataContext& context) final;
    };
}