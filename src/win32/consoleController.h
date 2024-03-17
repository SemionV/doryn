#pragma once

#include "base/domain/events/systemConsoleEventHub.h"

namespace dory::win32
{
    template<class TDataContext, typename TServiceLocator>
    class ConsoleController: public domain::Controller<TDataContext, TServiceLocator>
    {
    private:
        concurrency::IndividualProcessThread processThread;

    public:
        explicit ConsoleController(TServiceLocator& serviceLocator):
                domain::Controller<TDataContext, TServiceLocator>(serviceLocator)
        {}

        bool initialize(domain::entity::IdType referenceId, TDataContext& context) override
        {
            if(AllocConsole())
            {
                bindStdHandlesToConsole();
            }

            std::cout << "SystemConsole.connect()" << std::endl;

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
            this->services.consoleEventHub.submit(context);
        }

    protected:
        virtual void onKeyPressed(int key)
        {
            std::cout << std::this_thread::get_id() << ": add key pressed message: " << key << std::endl;

            domain::events::KeyPressedEventData eventData(key);
            this->services.consoleEventHub.addCase(std::forward<domain::events::KeyPressedEventData>(eventData));
        }

    private:
        void bindStdHandlesToConsole()
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
        }
    };

    template<class TDataContext>
    class ConsoleController2: public domain::Controller2<TDataContext>
    {
    private:
        concurrency::IndividualProcessThread processThread;

        using EventHubDispatcherType = domain::events::SystemConsoleEventHubDispatcher<TDataContext>;
        EventHubDispatcherType& consoleEventHub;

    public:
        explicit ConsoleController2(EventHubDispatcherType& consoleEventHub):
                consoleEventHub(consoleEventHub)
        {}

        bool initialize(domain::entity::IdType referenceId, TDataContext& context) override
        {
            if(AllocConsole())
            {
                bindStdHandlesToConsole();
            }

            std::cout << "SystemConsole.connect()" << std::endl;

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
        virtual void onKeyPressed(int key)
        {
            std::cout << std::this_thread::get_id() << ": add key pressed message: " << key << std::endl;

            domain::events::KeyPressedEventData eventData(key);
            consoleEventHub.addCase(std::forward<domain::events::KeyPressedEventData>(eventData));
        }

    private:
        void bindStdHandlesToConsole()
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
        }
    };
}