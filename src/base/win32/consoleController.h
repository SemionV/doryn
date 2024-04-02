#pragma once

#include "base/domain/events/systemConsoleEventHub.h"
#include "base/concurrency/individualProcessThread.h"

namespace dory::win32
{
    template<typename TDataContext>
    class ConsoleControllerFactory;

    template<class TDataContext>
    class ConsoleController: public domain::Controller<TDataContext>
    {
    private:
        concurrency::IndividualProcessThread processThread;

        using EventHubDispatcherType = domain::events::SystemConsoleEventHubDispatcher<TDataContext>;
        EventHubDispatcherType& consoleEventHub;

    public:
        using FactoryType = ConsoleControllerFactory<TDataContext>;

        explicit ConsoleController(EventHubDispatcherType& consoleEventHub):
                consoleEventHub(consoleEventHub)
        {}

        bool initialize(domain::entity::IdType referenceId, TDataContext& context) override
        {
            /*if(AllocConsole())
            {
                bindStdHandlesToConsole();
            }*/

            auto readInputTask = concurrency::allocateActionTask([this]()
                                                                 {
                                                                     int inputKey = getch();
                                                                     onKeyPressed(inputKey);
                                                                 });
            processThread.setRegularTask(readInputTask);

            processThread.run();

            return true;
        };

        void stop(domain::entity::IdType referenceId, TDataContext& context) override
        {
            processThread.stop();
        };

        void update(dory::domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, TDataContext& context) override
        {
            consoleEventHub.submit(context);
        }

    protected:
        void onKeyPressed(int key)
        {
            domain::events::KeyPressedEventData eventData(key);
            consoleEventHub.addCase(std::forward<domain::events::KeyPressedEventData>(eventData));
        }

    private:
        /*void bindStdHandlesToConsole()
        {
            //TODO: Add Error checking.

            // Redirect the CRT standard input, output, and error handles to the console
            freopen("CONIN$", "r", stdin);
            freopen("CONOUT$", "w", stderr);
            freopen("CONOUT$", "w", stdout);

            // Note that there is no CONERR$ file
            HANDLE hStdout = CreateFileA("CONOUT$",  GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                         NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            HANDLE hStdin = CreateFileA("CONIN$",  GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            SetStdHandle(STD_OUTPUT_HANDLE,hStdout);
            SetStdHandle(STD_ERROR_HANDLE,hStdout);
            SetStdHandle(STD_INPUT_HANDLE,hStdin);

            //Clear the error state for each of the C++ standard stream objects.
            std::wclog.clear();
            std::clog.clear();
            std::wcout.clear();
            std::cout.clear();
            std::wcerr.clear();
            std::cerr.clear();
            std::wcin.clear();
            std::cin.clear();
        }*/
    };

    template<typename TDataContext>
    class ConsoleControllerFactory: public IServiceFactory<ConsoleControllerFactory<TDataContext>>
    {
    private:
        using ControllerInterfaceType = domain::Controller<TDataContext>;

        using EventHubDispatcherType = domain::events::SystemConsoleEventHubDispatcher<TDataContext>;
        EventHubDispatcherType& consoleEventHub;

    public:
        explicit ConsoleControllerFactory(EventHubDispatcherType& consoleEventHub):
                consoleEventHub(consoleEventHub)
        {}

        std::shared_ptr<ControllerInterfaceType> createInstanceImpl()
        {
            return std::static_pointer_cast<ControllerInterfaceType>(std::make_shared<ConsoleController<TDataContext>>(consoleEventHub));
        }
    };
}