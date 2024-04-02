#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"

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

    template<typename TOutputStream>
    class Terminal: public ITerminal<Terminal<TOutputStream>>
    {
    private:
        const std::string_view commandModePrefix = "> ";
        TOutputStream& ostream;
        std::string currentCommand;
        bool commandMode = false;

    public:
        explicit Terminal(TOutputStream& ostream):
                ostream(ostream)
        {}

        template<typename T>
        void writeLineImpl(T message)
        {
            ostream << message << "\n";
        }

        template<typename T>
        void writeImpl(T message)
        {
            ostream << message;
        }

        void backspaceImpl()
        {
            if(commandMode && !currentCommand.empty())
            {
                currentCommand.erase(currentCommand.end() - 1);
                ostream << "\b \b";
                ostream.flush();
            }
        }

        void enterCommandModeImpl()
        {
            ostream << commandModePrefix;
            currentCommand = "";
            commandMode = true;
            ostream.flush();
        }

        void exitCommandModeImpl()
        {
            currentCommand = "";
            commandMode = false;
            ostream << "\n";
            ostream.flush();
        }

        void appendToCurrentCommandImpl(char symbol)
        {
            currentCommand += symbol;
            ostream << symbol;
            ostream.flush();
        }

        void clearCurrentCommandImpl()
        {
            auto count = currentCommand.size();

            for(std::size_t i = 0; i < count; ++i)
            {
                ostream << "\b";
            }
            for(std::size_t i = 0; i < count; ++i)
            {
                ostream << " ";
            }
            for(std::size_t i = 0; i < count; ++i)
            {
                ostream << "\b";
            }

            currentCommand = "";
            ostream.flush();
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
