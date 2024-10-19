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
            auto outputDevice = _registry.get<devices::IStandardIODevice>();
            if(outputDevice)
            {
                outputDevice->out(message);
            }
            enterCommandMode();
        }
        else
        {
            auto outputDevice = _registry.get<devices::IStandardIODevice>();
            if(outputDevice)
            {
                outputDevice->out(message);
            }
        }
    }

    void TerminalDevice::writeLine(const std::string& message)
    {
        auto data = std::string{message} + "\n";

        if(commandMode)
        {
            exitCommandMode();
            auto outputDevice = _registry.get<devices::IStandardIODevice>();
            if(outputDevice)
            {
                outputDevice->out(message);
            }
            enterCommandMode();
        }
        else
        {
            auto outputDevice = _registry.get<devices::IStandardIODevice>();
            if(outputDevice)
            {
                outputDevice->out(message);
            }
        }
    }

    void TerminalDevice::enterCommandMode()
    {

    }

    void TerminalDevice::exitCommandMode()
    {

    }

    bool TerminalDevice::isCommandMode()
    {

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
            /*inputEventHub->detach(_inputListenerId);
            inputEventHub->attach([this](resources::DataContext& context, events::io::KeyPressEvent& event)
            {
                onKeyPress(context, event);
            });*/
        }
    }

    void TerminalDevice::onKeyPress(resources::DataContext& context, events::io::KeyPressEvent& eventData)
    {
        /*switch(eventData.keyCode)
        {
            case events::io::KeyCode::Return:
            {
                onPressReturn(context, eventData);
                break;
            }
            case events::io::KeyCode::Backspace:
            {
                onPressBackspace(context, eventData);
                break;
            }
            case events::io::KeyCode::Escape:
            {
                onPressEscape(context, eventData);
                break;
            }
            case events::io::KeyCode::Terminate:
            {
                onPressTerminate(context, eventData);
                break;
            }
            case events::io::KeyCode::Character:
            {
                onEnterSymbol(context, eventData);
                break;
            }
            case events::io::KeyCode::Unknown:
            default:
                break;
        }*/
    }
}