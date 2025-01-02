#include <dory/core/devices/terminalDevice.h>

namespace dory::core::devices
{
    TerminalDevice::TerminalDevice(Registry& registry):
        _registry(registry)
    {}

    void TerminalDevice::write(const std::string& message)
    {
        if(commandMode)
        {
            exitCommandMode();
            out(message);
            enterCommandMode();
        }
        else
        {
            out(message);
        }
    }

    void TerminalDevice::writeLine(const std::string& message)
    {
        auto data = std::string{message} + "\n";

        if(commandMode)
        {
            exitCommandMode();
            out(data);
            enterCommandMode();
        }
        else
        {
            out(data);
        }
    }

    void TerminalDevice::enterCommandMode()
    {
        out(commandModePrefix);
        currentCommand = "";
        commandMode = true;
    }

    void TerminalDevice::exitCommandMode()
    {
        currentCommand = "";
        commandMode = false;
        out("\n");
    }

    bool TerminalDevice::isCommandMode()
    {
        return commandMode;
    }

    void TerminalDevice::connect(resources::DataContext& context)
    {
        auto inputEventHub = _registry.get<events::io::Bundle::IListener>();
        if(inputEventHub)
        {
            _inputListenerId = inputEventHub->attach([this](resources::DataContext& context, events::io::KeyPressEvent& event)
            {
                onKeyPress(context, event);
            });
        }
    }

    void TerminalDevice::disconnect(resources::DataContext& context)
    {
        auto inputEventHub = _registry.get<events::io::Bundle::IListener>();
        if(inputEventHub)
        {
            inputEventHub->detach(_inputListenerId, events::io::KeyPressEvent{});
        }
    }

    void TerminalDevice::onKeyPress(resources::DataContext& context, events::io::KeyPressEvent& eventData)
    {
        switch(eventData.keyCode)
        {
            case events::KeyCode::Return:
            {
                onPressReturn(context, eventData);
                break;
            }
            case events::KeyCode::Backspace:
            {
                onPressBackspace(context, eventData);
                break;
            }
            case events::KeyCode::Escape:
            {
                onPressEscape(context, eventData);
                break;
            }
            case events::KeyCode::Terminate:
            {
                onPressTerminate(context, eventData);
                break;
            }
            case events::KeyCode::Character:
            {
                onEnterSymbol(context, eventData);
                break;
            }
            case events::KeyCode::Unknown:
            default:
                break;
        }
    }

    void TerminalDevice::out(const std::string& message)
    {
        auto outputDevice = _registry.get<devices::IStandardIODevice>();
        if(outputDevice)
        {
            outputDevice->out(message);
        }
    }

    void TerminalDevice::appendToCurrentCommand(char symbol)
    {
        currentCommand += symbol;
        out(std::string{symbol});
    }

    void TerminalDevice::clearCurrentCommand()
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

        out(message);
        currentCommand = "";
    }

    void TerminalDevice::onPressReturn(resources::DataContext &context, events::io::KeyPressEvent eventData)
    {
        if(commandMode)
        {
            auto command = currentCommand;
            exitCommandMode();

            auto scriptService = _registry.get<services::IScriptService>();
            if(scriptService)
            {
                scriptService->runScript(context, command, {});
            }

            enterCommandMode();
        }
    }

    void TerminalDevice::onPressEscape(resources::DataContext &context, events::io::KeyPressEvent eventData)
    {
        if(commandMode)
        {
            clearCurrentCommand();
        }
    }

    void TerminalDevice::onPressBackspace(resources::DataContext &context, events::io::KeyPressEvent eventData)
    {
        if(commandMode && !currentCommand.empty())
        {
            currentCommand.erase(currentCommand.end() - 1);
            out("\b \b");
        }
    }

    void TerminalDevice::onPressTerminate(resources::DataContext &context, events::io::KeyPressEvent eventData)
    {
        auto eventDispatcher = _registry.get<events::application::Bundle::IDispatcher>();
        if(eventDispatcher)
        {
            eventDispatcher->fire(context, events::application::Exit{});
        }
    }

    void TerminalDevice::onEnterSymbol(resources::DataContext &context, events::io::KeyPressEvent eventData)
    {
        if(commandMode)
        {
            appendToCurrentCommand(eventData.character);
        }
    }
}