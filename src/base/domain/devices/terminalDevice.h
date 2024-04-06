#pragma once

#include "device.h"
#include "base/domain/events/inputEventHub.h"
//#include "base/domain/devices/device.h"

namespace dory::domain::devices
{
    template<typename TImpelementation, typename TOutputData>
    struct ITerminal
    {
        void write(const TOutputData& data)
        {
            toImplementation<TImpelementation>(this)->writeImpl(data);
        }

        void writeLine(const TOutputData& data)
        {
            toImplementation<TImpelementation>(this)->writeLineImpl(data);
        }
    };

    template<typename TDataContext, typename TOutputDevice>
    class TerminalDevice: Uncopyable,
            public IDevice<TerminalDevice<TDataContext, TOutputDevice>, TDataContext>,
            public ITerminal<TerminalDevice<TDataContext, TOutputDevice>, std::string>
    {
    private:
        const std::string commandModePrefix = "> ";
        std::string currentCommand;
        bool commandMode = false;

        using OutputDeviceType = IStandartOutputDevice<TOutputDevice, std::string>;
        OutputDeviceType& outputDevice;

        void enterCommandMode()
        {
            outputDevice.out(commandModePrefix);
            currentCommand = "";
            commandMode = true;
        }

        void exitCommandMode()
        {
            currentCommand = "";
            commandMode = false;
            outputDevice.out("\n");
        }

    public:
        explicit TerminalDevice(OutputDeviceType& outputDevice):
            outputDevice(outputDevice)
        {}

        template<typename T>
        void writeImpl(T message)
        {
            if(commandMode)
            {
                exitCommandMode();
                outputDevice.out(message);
                enterCommandMode();
            }
            else
            {
                sendToOutputDevice(message);
            }
        }

        template<typename T>
        void writeLineImpl(T message)
        {
            auto data = std::string{message} + "\n";

            if(commandMode)
            {
                exitCommandMode();
                outputDevice.out(data);
                enterCommandMode();
            }
            else
            {
                outputDevice.out(data);
            }
        }
    };
}