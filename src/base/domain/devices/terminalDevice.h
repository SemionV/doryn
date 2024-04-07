#pragma once

#include "device.h"
#include "base/domain/events/inputEventHub.h"

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

        void enterCommandMode()
        {
            toImplementation<TImpelementation>(this)->enterCommandModeImpl();
        }

        void exitCommandMode()
        {
            toImplementation<TImpelementation>(this)->exitCommandModeImpl();
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

        using InputEventHubType = events::InputEventHub<TDataContext>;
        InputEventHubType& inputEventHub;

    public:
        explicit TerminalDevice(OutputDeviceType& outputDevice, InputEventHubType& inputEventHub):
            outputDevice(outputDevice),
            inputEventHub(inputEventHub)
        {}

        template<typename T>
        void writeImpl(T message)
        {
            if(commandMode)
            {
                exitCommandModeImpl();
                outputDevice.out(message);
                enterCommandModeImpl();
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
                exitCommandModeImpl();
                outputDevice.out(data);
                enterCommandModeImpl();
            }
            else
            {
                outputDevice.out(data);
            }
        }

        void connectImpl(TDataContext& context)
        {
            inputEventHub.onPressReturn().attachHandler(this, &TerminalDevice::onPressReturn);
            inputEventHub.onPressSymbol().attachHandler(this, &TerminalDevice::onPressSymbol);
        }

        void disconnectImpl(TDataContext& context)
        {
        }

        void enterCommandModeImpl()
        {
            outputDevice.out(commandModePrefix);
            currentCommand = "";
            commandMode = true;
        }

        void exitCommandModeImpl()
        {
            currentCommand = "";
            commandMode = false;
            outputDevice.out("\n");
        }

    private:
        void appendToCurrentCommand(char symbol)
        {
            currentCommand += symbol;
            outputDevice.out(std::string{symbol});
        }

        void onPressReturn(TDataContext& context, events::io::PressEnterEventData eventData)
        {
            if(commandMode)
            {
                auto command = currentCommand;
                exitCommandModeImpl();

                if(command == "exit")
                {
                    outputDevice.out("-\u001B[31mexit\u001B[0m-\n");
                }

                enterCommandModeImpl();
            }
        }

        void onPressSymbol(TDataContext& context, events::io::PressSymbolEventData eventData)
        {
            if(commandMode)
            {
                appendToCurrentCommand(eventData.symbol);
            }
        }
    };
}