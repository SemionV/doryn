#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"

namespace dory::domain::services
{
    template<typename TImplementation>
    struct ITerminal: Uncopyable, public StaticInterface<TImplementation>
    {
        void writeLine(std::string_view message)
        {
            this->toImplementation()->writeLineImpl(message);
        }

        void write(char symbol)
        {
            this->toImplementation()->writeImpl(symbol);
        }

        void write(std::string_view message)
        {
            this->toImplementation()->writeImpl(message);
        }

        void enterCommandMode()
        {
            this->toImplementation()->enterCommandModeImpl();
        }

        void clearCurrentCommand()
        {
            this->toImplementation()->clearCurrentCommandImpl();
        }

        void appendToCurrentCommand(char symbol)
        {
            this->toImplementation()->appendToCurrentCommandImpl(symbol);
        }
    };

    template<typename TOutputStream>
    class Terminal: public ITerminal<Terminal<TOutputStream>>
    {
    private:
        const std::string_view commandModePrefix = "> ";
        TOutputStream& ostream;
        std::string currentCommand;

    public:
        explicit Terminal(TOutputStream& ostream):
                ostream(ostream)
        {}

        void writeLineImpl(std::string_view message)
        {
            ostream << message << "\n";
        }

        void writeImpl(std::string_view message)
        {
            ostream << message;
        }

        void writeImpl(char symbol)
        {
            ostream << symbol;
        }

        void enterCommandModeImpl()
        {
            ostream << commandModePrefix;
            currentCommand = "";
        }

        void appendToCurrentCommandImpl(char symbol)
        {
            currentCommand += symbol;
            ostream << symbol;
        }

        void clearCurrentCommandImpl()
        {
            ostream << "\r";
            auto count = currentCommand.size() + commandModePrefix.size();
            for(std::size_t i = 0; i < count; ++i)
            {
                ostream << " ";
            }
            ostream << "\r";
            enterCommandModeImpl();
        }
    };
}
