#pragma once

#include "base/domain/services/terminal.h"
#include "base/domain/events/systemConsoleEventHub.h"
#include "base/domain/events/applicationEventHub.h"

namespace dory::domain::logic
{
    template<typename TImplementation, typename TDataContext>
    struct ICliManager: Uncopyable, public StaticInterface<TImplementation>
    {
        void initialize(TDataContext& dataContext)
        {
            this->toImplementation()->initializeImpl(dataContext);
        }
        void stop(TDataContext& dataContext)
        {
            this->toImplementation()->stopImpl(dataContext);
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

        using ApplicationEventDispatcherType = events::ApplicationEventDispatcher<TDataContext>;
        ApplicationEventDispatcherType& applicationEventDispatcher;

    public:
        explicit CliManager(TerminalType& terminal, TerminalEventHubType& terminalEventHub, ApplicationEventDispatcherType& applicationEventDispatcher):
                terminal(terminal), terminalEventHub(terminalEventHub), applicationEventDispatcher(applicationEventDispatcher)
        {}

        void initializeImpl(TDataContext& dataContext)
        {
            terminalEventHub.onKeyPressed().attachHandler(this, &CliManager::onKeyPressed);
            terminal.enterCommandMode();
        }
        void stopImpl(TDataContext& dataContext)
        {
            if(terminal.isCommandMode())
            {
                terminal.exitCommandMode();
            }
        }

    private:
        void onKeyPressed(TDataContext& context, events::KeyPressedEventData& eventData)
        {
            /*std::cout << (int)eventData.keyPressed << "\n";
            return;*/

            if(eventData.keyPressed == 3)//CTRL+C
            {
                applicationEventDispatcher.fire(context, events::ApplicationExitEventData{});
            }
            else if(eventData.keyPressed == 27)//ESC
            {
                if(terminal.isCommandMode())
                {
                    terminal.clearCurrentCommand();
                }
            }
            else if(eventData.keyPressed == 127)//BACKSPACE
            {
                terminal.backspace();
            }
            else if(eventData.keyPressed == 10)//ENTER
            {
                if(terminal.isCommandMode())
                {
                    auto command = terminal.getCurrentCommand();
                    terminal.exitCommandMode();

                    if(command == "exit")
                    {
                        terminal.writeLine("-\u001B[31mexit\u001B[0m-");
                        applicationEventDispatcher.fire(context, events::ApplicationExitEventData{});
                    }
                    else
                    {
                        terminal.enterCommandMode();
                    }
                }
            }
            else if(eventData.keyPressed != 0)
            {
                if(terminal.isCommandMode())
                {
                    terminal.appendToCurrentCommand((char)eventData.keyPressed);
                }
            }
        }
    };
}