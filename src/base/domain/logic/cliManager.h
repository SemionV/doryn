#pragma once

#include "base/domain/services/terminal.h"
#include "base/domain/events/systemConsoleEventHub.h"

namespace dory::domain::logic
{
    template<typename TImplementation, typename TDataContext>
    struct ICliManager: Uncopyable, public StaticInterface<TImplementation>
    {
        void initialize(TDataContext& dataContext)
        {
            this->toImplementation()->initializeImpl(dataContext);
        }
    };

    template<typename TDataContext, typename TTerminal>
    class CliManager: public ICliManager<CliManager<TDataContext, TTerminal>, TDataContext>
    {
    private:
        using TerminalType = services::ITerminal<TTerminal>;
        TerminalType& terminal;

        using TerminalEventHubType = events::SystemConsoleEventHub<TDataContext>;
        TerminalEventHubType& terminalEventHub;

    public:
        explicit CliManager(TerminalType& terminal, TerminalEventHubType& terminalEventHub):
                terminal(terminal), terminalEventHub(terminalEventHub)
        {}

        void initializeImpl(TDataContext& dataContext)
        {
            terminalEventHub.onKeyPressed().attachHandler(this, &CliManager::onKeyPressed);
            terminal.enterCommandMode();
        }

    private:
        void onKeyPressed(TDataContext& context, events::KeyPressedEventData& eventData)
        {
            if(eventData.keyPressed == 27)
            {
                terminal.clearCurrentCommand();
            }
            else if(eventData.keyPressed != 0)
            {
                terminal.appendToCurrentCommand((char)eventData.keyPressed);
            }
        }
    };
}