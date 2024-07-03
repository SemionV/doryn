#pragma once

#include <dory/engine/dependencies.h>
#include <dory/engine/domain/events/hub.h>
#include "device.h"

namespace dory::domain::devices
{
    static struct termios oldt, currentt;

    template<typename TDataContext>
    class ConsoleIODevice: NonCopyable,
                               public IDevice<ConsoleIODevice<TDataContext>, TDataContext>,
                               public IStandartOutputDevice<ConsoleIODevice<TDataContext>, std::string>
    {
    private:
        using InputEventDispatcherType =  events::io::Dispatcher<TDataContext>;
        InputEventDispatcherType& inputEventDispatcher;

        std::jthread pollingThread;
        bool connected = false;
        int pipes[2];

        void onKeyPressed(TDataContext& context, int key)
        {
            if(key == 3)//CTRL+C
            {
                inputEventDispatcher.charge(events::io::KeyPressEvent{ events::io::KeyCode::Terminate });
            }
            else if(key == 27)//ESC
            {
                inputEventDispatcher.charge(events::io::KeyPressEvent{ events::io::KeyCode::Escape });
            }
            else if(key == 127)//BACKSPACE
            {
                inputEventDispatcher.charge(events::io::KeyPressEvent{ events::io::KeyCode::Backspace });
            }
            else if(key == 10)//END OF LINE
            {
                inputEventDispatcher.charge(events::io::KeyPressEvent{ events::io::KeyCode::Return });
            }
            else if(key != 0)// Character
            {
                inputEventDispatcher.charge(events::io::KeyPressEvent{ events::io::KeyCode::Character, key });
            }
            else
            {
                inputEventDispatcher.charge(events::io::KeyPressEvent{ events::io::KeyCode::Unknown });
            }
        }

    public:
        explicit ConsoleIODevice(InputEventDispatcherType& inputEventDispatcher):
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
                tcgetattr(STDIN_FILENO, &oldt);
                currentt = oldt;
                currentt.c_lflag &= ~ICANON; /* disable buffered i/o */
                currentt.c_lflag &= ~ECHO; /* set no echo mode */
                tcsetattr(STDIN_FILENO, TCSANOW, &currentt);

                pipe(pipes);

                pollingThread = std::jthread([this, &context](const std::stop_token& stoken, void* pipe)
                {
                    //see more about this thread unblocking technic here: https://stackoverflow.com/questions/11513593/cancelling-getchar
                    int readPipe = *(int*)pipe;
                    fd_set rfds;
                    int inputKey;

                    while(!stoken.stop_requested())
                    {
                        FD_ZERO(&rfds);
                        FD_SET(STDIN_FILENO, &rfds);
                        FD_SET(readPipe, &rfds);

                        while (select(readPipe + 1, &rfds, NULL, NULL, NULL) == 0);

                        if (FD_ISSET(readPipe, &rfds)) {
                            close(readPipe);
                            break;
                        }

                        if (FD_ISSET(STDIN_FILENO, &rfds)) {
                            if (read(STDIN_FILENO, &inputKey, sizeof(inputKey)) > 0 && !stoken.stop_requested()) {
                                onKeyPressed(context, inputKey);
                            }
                        }
                    }
                }, &pipes[0]);

                connected = true;
            }
        }

        void disconnectImpl(TDataContext& context)
        {
            if(connected)
            {
                tcsetattr(0, TCSANOW, &oldt);
                pollingThread.request_stop();
                close(pipes[1]);
                pollingThread.join();
                connected = false;
            }
        }
    };
}