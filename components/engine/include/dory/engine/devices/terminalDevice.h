#pragma once

#include <dory/engine/resources/eventTypes.h>
#include "device.h"

namespace dory::engine::devices
{
    template<typename TImplementation, typename TOutputData = std::string>
    struct ITerminal
    {
        void write(const TOutputData& data)
        {
            toImplementation<TImplementation>(this)->writeImpl(data);
        }

        void writeLine(const TOutputData& data)
        {
            toImplementation<TImplementation>(this)->writeLineImpl(data);
        }

        void enterCommandMode()
        {
            toImplementation<TImplementation>(this)->enterCommandModeImpl();
        }

        void exitCommandMode()
        {
            toImplementation<TImplementation>(this)->exitCommandModeImpl();
        }

        bool isCommandMode()
        {
            return toImplementation<TImplementation>(this)->isCommandModeImpl();
        }
    };

    template<typename TDataContext, typename TOutputDevice>
    class TerminalDevice: NonCopyable,
                          public IDevice<TerminalDevice<TDataContext, TOutputDevice>, TDataContext>,
                          public ITerminal<TerminalDevice<TDataContext, TOutputDevice>>
    {
    private:
        const std::string commandModePrefix = "> ";
        std::string currentCommand;
        bool commandMode = false;

        using OutputDeviceType = IStandardOutputDevice<TOutputDevice, std::string>;
        OutputDeviceType& outputDevice;

        using InputEventHubType = resources::eventTypes::io::Hub<TDataContext>;
        InputEventHubType& inputEventHub;

        using ScriptEventDispatcherType = resources::eventTypes::script::Dispatcher<TDataContext>;
        ScriptEventDispatcherType& scriptEventDispatcher;

        using ApplicationEventDispatcherType = resources::eventTypes::application::Dispatcher<TDataContext>;
        ApplicationEventDispatcherType& applicationEventDispatcher;

    public:
        explicit TerminalDevice(OutputDeviceType& outputDevice,
                                InputEventHubType& inputEventHub,
                                ScriptEventDispatcherType& scriptEventDispatcher,
                                ApplicationEventDispatcherType& applicationEventDispatcher):
            outputDevice(outputDevice),
            inputEventHub(inputEventHub),
            scriptEventDispatcher(scriptEventDispatcher),
            applicationEventDispatcher(applicationEventDispatcher)
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
            inputEventHub.attach(this, &TerminalDevice::onKeyPress);
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

        bool isCommandModeImpl()
        {
            return commandMode;
        }

    private:
        void appendToCurrentCommand(char symbol)
        {
            currentCommand += symbol;
            outputDevice.out(std::string{symbol});
        }

        void clearCurrentCommand()
        {
            auto count = currentCommand.size();

            std::string message;
            for(std::size_t i = 0; i < count; ++i)
            {
                message += "\b";
            }
            for(std::size_t i = 0; i < count; ++i)
            {
                message += " ";
            }
            for(std::size_t i = 0; i < count; ++i)
            {
                message += "\b";
            }

            outputDevice.out(message);
            currentCommand = "";
        }

        void onPressReturn(TDataContext& context, resources::eventTypes::io::KeyPressEvent eventData)
        {
            if(commandMode)
            {
                auto command = currentCommand;
                exitCommandModeImpl();

                scriptEventDispatcher.fire(context, resources::eventTypes::script::Run{command});

                enterCommandModeImpl();
            }
        }

        void onPressEscape(TDataContext& context, resources::eventTypes::io::KeyPressEvent eventData)
        {
            if(commandMode)
            {
                clearCurrentCommand();
            }
        }

        void onPressBackspace(TDataContext& context, resources::eventTypes::io::KeyPressEvent eventData)
        {
            if(commandMode && !currentCommand.empty())
            {
                currentCommand.erase(currentCommand.end() - 1);
                outputDevice.out("\b \b");
            }
        }

        void onPressTerminate(TDataContext& context, resources::eventTypes::io::KeyPressEvent eventData)
        {
            applicationEventDispatcher.fire(context, resources::eventTypes::application::Exit{});
        }

        void onEnterSymbol(TDataContext& context, resources::eventTypes::io::KeyPressEvent eventData)
        {
            if(commandMode)
            {
                appendToCurrentCommand(eventData.character);
            }
        }

        void onKeyPress(TDataContext& context, resources::eventTypes::io::KeyPressEvent& eventData)
        {
            switch(eventData.keyCode)
            {
                case resources::eventTypes::io::KeyCode::Return:
                {
                    onPressReturn(context, eventData);
                    break;
                }
                case resources::eventTypes::io::KeyCode::Backspace:
                {
                    onPressBackspace(context, eventData);
                    break;
                }
                case resources::eventTypes::io::KeyCode::Escape:
                {
                    onPressEscape(context, eventData);
                    break;
                }
                case resources::eventTypes::io::KeyCode::Terminate:
                {
                    onPressTerminate(context, eventData);
                    break;
                }
                case resources::eventTypes::io::KeyCode::Character:
                {
                    onEnterSymbol(context, eventData);
                    break;
                }
                case resources::eventTypes::io::KeyCode::Unknown:
                default:
                    break;
            }
        }
    };
}