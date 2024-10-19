#pragma once

#include <memory>
#include <dory/core/devices/iTerminalDevice.h>
#include <dory/core/devices/iStandardIODevice.h>

#include <dory/core/registry.h>

namespace dory::core::devices
{
    class TerminalDevice: public ITerminalDevice
    {
    private:
        const std::string commandModePrefix = "> ";
        std::string currentCommand;
        bool commandMode = false;
        std::size_t _inputListenerId{};

        Registry& _registry;

        /*std::shared_ptr<IStandardIODevice> _outputDevice;
        std::shared_ptr<events::io::IEventHub> _inputEventHub;
        std::shared_ptr<events::script::IEventDispatcher> _scriptEventDispatcher;
        std::shared_ptr<events::application::IEventDispatcher> _applicationEventDispatcher;*/

    public:
        explicit TerminalDevice(Registry& registry);

        void write(const std::string& message) final;
        void writeLine(const std::string& message) final;
        void enterCommandMode() final;
        void exitCommandMode() final;
        bool isCommandMode() final;
        void connect(resources::DataContext& context) final;
        void disconnect(resources::DataContext& context) final;

    private:
        void onKeyPress(resources::DataContext& context, events::io::KeyPressEvent& eventData);
    };
}
