#pragma once

#include "device.h"
#include "base/domain/events/inputEventHub.h"

namespace dory::domain::devices
{
    template<typename TDataContext>
    class ConsoleIODeviceWin32: Uncopyable,
            public IDevice<ConsoleIODeviceWin32<TDataContext>, TDataContext>,
            public IStandartOutputDevice<ConsoleIODeviceWin32<TDataContext>, std::string>
    {
    private:
        using InputEventDispatcherType =  events::InputEventDispatcher<TDataContext, int, std::string>;
        InputEventDispatcherType& inputEventDispatcher;

        std::jthread pollingThread;

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

        void onKeyPressed(int key)
        {
            inputEventDispatcher.addCase(key);
        }

    public:
        explicit ConsoleIODeviceWin32(InputEventDispatcherType& inputEventDispatcher):
                inputEventDispatcher(inputEventDispatcher)
        {}

        void outImpl(const std::string& data)
        {
            std::cout << data;
        }

        void flushImpl()
        {
            std::cout.flush();
        }

        void connectImpl(TDataContext& context)
        {
            if(AllocConsole())
            {
                bindStdHandlesToConsole();
            }

            //disable echo: https://forums.codeguru.com/showthread.php?466009-Reading-from-stdin-(without-echo)
            HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
            DWORD mode = 0;
            GetConsoleMode(hStdin, &mode);
            SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));

            pollingThread = std::jthread([this](const std::stop_token& stoken)
            {
                while(!stoken.stop_requested())
                {
                    int inputKey = getchar();

                    if(!stoken.stop_requested() && inputKey != EOF)
                    {
                        onKeyPressed(inputKey);
                    }
                    else
                    {
                        break;
                    }
                }
            });
        }

        void disconnectImpl(TDataContext& context)
        {
            pollingThread.request_stop();
            pollingThread.detach();
        }
    };
}
