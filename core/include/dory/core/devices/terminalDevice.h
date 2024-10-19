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
        inline void out(const std::string& message);
        void onKeyPress(resources::DataContext& context, events::io::KeyPressEvent& eventData);
        inline void appendToCurrentCommand(char symbol);
        inline void clearCurrentCommand();
        inline void onPressReturn(resources::DataContext& context, events::io::KeyPressEvent eventData);
        inline void onPressEscape(resources::DataContext& context, events::io::KeyPressEvent eventData);
        inline void onPressBackspace(resources::DataContext& context, events::io::KeyPressEvent eventData);
        inline void onPressTerminate(resources::DataContext& context, events::io::KeyPressEvent eventData);
        inline void onEnterSymbol(resources::DataContext& context, events::io::KeyPressEvent eventData);
    };
}
