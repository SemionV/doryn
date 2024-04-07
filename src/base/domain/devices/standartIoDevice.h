#pragma once

#include "device.h"
#include "base/domain/events/inputEventHub.h"
#include "base/win32/dependencies.h"

namespace dory::domain::devices
{
    template<typename TDataContext>
    class ConsoleIODeviceWin32: Uncopyable,
            public IDevice<ConsoleIODeviceWin32<TDataContext>, TDataContext>,
            public IStandartOutputDevice<ConsoleIODeviceWin32<TDataContext>, std::string>
    {
    private:
        using InputEventDispatcherType =  events::InputEventDispatcher<TDataContext>;
        InputEventDispatcherType& inputEventDispatcher;

        std::jthread pollingThread;
        bool connected = false;

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

        void onKeyPressed(TDataContext& context, INPUT_RECORD inputRecord)
        {
            if(inputRecord.Event.KeyEvent.uChar.AsciiChar == 3)//CTRL+C
            {
                inputEventDispatcher.addCase(context, events::io::KeyPressEvent{ events::io::KeyCode::Terminate });
            }
            else if(inputRecord.Event.KeyEvent.wVirtualKeyCode == 27)//ESC
            {
                inputEventDispatcher.addCase(context, events::io::KeyPressEvent{ events::io::KeyCode::Escape });
            }
            else if(inputRecord.Event.KeyEvent.wVirtualKeyCode == 8)//BACKSPACE
            {
                inputEventDispatcher.addCase(context, events::io::KeyPressEvent{ events::io::KeyCode::Backspace });
            }
            else if(inputRecord.Event.KeyEvent.wVirtualKeyCode == 13)//RETURN
            {
                inputEventDispatcher.addCase(context, events::io::KeyPressEvent{ events::io::KeyCode::Return });
            }
            else if(inputRecord.Event.KeyEvent.uChar.AsciiChar != 0)// Character
            {
                inputEventDispatcher.addCase(context, events::io::KeyPressEvent{ events::io::KeyCode::Character, inputRecord.Event.KeyEvent.uChar.AsciiChar });
            }
            else
            {
                inputEventDispatcher.addCase(context, events::io::KeyPressEvent{ events::io::KeyCode::Unknown });
            }
        }

    public:
        explicit ConsoleIODeviceWin32(InputEventDispatcherType& inputEventDispatcher):
                inputEventDispatcher(inputEventDispatcher)
        {}

        void outImpl(const std::string& data)
        {
            if(connected)
            {
                std::cout << data;
            }
        }

        void flushImpl()
        {
            if(connected)
            {
                std::cout.flush();
            }
        }

        void connectImpl(TDataContext& context)
        {
            if(!connected)
            {
                if(AllocConsole())
                {
                    bindStdHandlesToConsole();
                }

                //disable echo: https://forums.codeguru.com/showthread.php?466009-Reading-from-stdin-(without-echo)
                HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
                DWORD oldMode = 0;
                GetConsoleMode(hStdin, &oldMode);
                DWORD mode = oldMode;
                SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT) & (~ENABLE_LINE_INPUT) & (~ENABLE_PROCESSED_INPUT));

                pollingThread = std::jthread([this, &context, hStdin, oldMode](const std::stop_token& stoken)
                {
                    INPUT_RECORD inputRecord;
                    long unsigned int recordsRead = 0;

                    while(!stoken.stop_requested())
                    {
                        if(ReadConsoleInputA(hStdin, &inputRecord, 1, &recordsRead))
                        {
                            if(stoken.stop_requested())
                            {
                                break;
                            }

                            switch(inputRecord.EventType)
                            {
                                case KEY_EVENT:
                                    if(inputRecord.Event.KeyEvent.bKeyDown)
                                    {
                                        onKeyPressed(context, inputRecord);
                                    }
                                    break;
                                case MOUSE_EVENT:
                                case WINDOW_BUFFER_SIZE_EVENT:
                                case FOCUS_EVENT:
                                case MENU_EVENT:
                                default:
                                    break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }

                    SetConsoleMode(hStdin, oldMode);
                });

                connected = true;
            }
        }

        void disconnectImpl(TDataContext& context)
        {
            if(connected)
            {
                pollingThread.request_stop();

                //Send input to console to unblock the polling thread
                HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
                INPUT_RECORD eventData;
                eventData.EventType = KEY_EVENT;
                eventData.Event.KeyEvent.bKeyDown = true;
                eventData.Event.KeyEvent.uChar.AsciiChar = VK_RETURN;
                eventData.Event.KeyEvent.dwControlKeyState = 0;
                eventData.Event.KeyEvent.wRepeatCount = 0;
                eventData.Event.KeyEvent.wVirtualKeyCode = 0;
                long unsigned int keys_written = 0;
                if(WriteConsoleInputA(hStdin, &eventData, 1, &keys_written))
                {
                    pollingThread.join();
                }
                else
                {
                    pollingThread.detach();
                }
                connected = false;
            }
        }
    };
}
