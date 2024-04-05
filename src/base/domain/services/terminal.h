#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/domain/events/ioEventHub.h"

namespace dory::domain::services
{
    template<typename TImplementation>
    struct ITerminal: Uncopyable, public StaticInterface<TImplementation>
    {
        template<typename T>
        void writeLine(T message)
        {
            this->toImplementation()->writeLineImpl(message);
        }

        template<typename T>
        void write(T message)
        {
            this->toImplementation()->writeImpl(message);
        }

        void backspace()
        {
            this->toImplementation()->backspaceImpl();
        }

        void enterCommandMode()
        {
            this->toImplementation()->enterCommandModeImpl();
        }

        void exitCommandMode()
        {
            this->toImplementation()->exitCommandModeImpl();
        }

        void clearCurrentCommand()
        {
            this->toImplementation()->clearCurrentCommandImpl();
        }

        void appendToCurrentCommand(char symbol)
        {
            this->toImplementation()->appendToCurrentCommandImpl(symbol);
        }

        bool isCommandMode()
        {
            return this->toImplementation()->isCommandModeImpl();
        }

        std::string getCurrentCommand()
        {
            return this->toImplementation()->getCurrentCommandImpl();
        }
    };

    template<typename TDataContext, typename TInputEventData, typename TOutputEventData>
    class Terminal: public ITerminal<Terminal<TDataContext, TInputEventData, TOutputEventData>>
    {
    private:
        using IOEventHubDispatcherType = events::IOEventHubDispatcher<TDataContext, TInputEventData, TOutputEventData>;
        IOEventHubDispatcherType& ioEventDispatcher;

        const std::string commandModePrefix = "> ";
        std::string currentCommand;
        bool commandMode = false;

        void sendToOutputDevice(const TOutputEventData& data)
        {
            ioEventDispatcher.addCase(data);
        }

    public:
        explicit Terminal(IOEventHubDispatcherType& ioEventDispatcher):
                ioEventDispatcher(ioEventDispatcher)
        {}

        template<typename T>
        void writeLineImpl(T message)
        {
            auto data = std::string{message} + "\n";

            if(commandMode)
            {
                exitCommandModeImpl();
                sendToOutputDevice(data);
                enterCommandModeImpl();
            }
            else
            {
                sendToOutputDevice(data);
            }
        }

        template<typename T>
        void writeImpl(T message)
        {
            if(commandMode)
            {
                exitCommandModeImpl();
                sendToOutputDevice(message);
                enterCommandModeImpl();
            }
            else
            {
                sendToOutputDevice(message);
            }
        }

        void backspaceImpl()
        {
            if(commandMode && !currentCommand.empty())
            {
                currentCommand.erase(currentCommand.end() - 1);
                sendToOutputDevice("\b \b");
            }
        }

        void enterCommandModeImpl()
        {
            sendToOutputDevice(commandModePrefix);
            currentCommand = "";
            commandMode = true;
        }

        void exitCommandModeImpl()
        {
            currentCommand = "";
            commandMode = false;
            sendToOutputDevice("\n");
        }

        void appendToCurrentCommandImpl(char symbol)
        {
            currentCommand += symbol;
            sendToOutputDevice(std::string{symbol});
        }

        void clearCurrentCommandImpl()
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

            sendToOutputDevice(message);
            currentCommand = "";
        }

        bool isCommandModeImpl()
        {
            return commandMode;
        }

        std::string getCurrentCommandImpl()
        {
            return currentCommand;
        }
    };
}
