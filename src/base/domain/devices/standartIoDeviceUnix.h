#pragma once

#include "device.h"
#include "base/domain/events/inputEventHub.h"
#include "base/unix/dependencies.h"

namespace dory::domain::devices
{
    static struct termios oldt, currentt;

    template<typename TDataContext>
    class ConsoleIODeviceUnix: Uncopyable,
                                public IDevice<ConsoleIODeviceUnix<TDataContext>, TDataContext>,
                                public IStandartOutputDevice<ConsoleIODeviceUnix<TDataContext>, std::string>
    {
    private:
        using InputEventDispatcherType =  events::InputEventDispatcher<TDataContext>;
        InputEventDispatcherType& inputEventDispatcher;

        std::jthread pollingThread;
        bool connected = false;
        //int pipes[2];

        void onKeyPressed(TDataContext& context, int key)
        {
            if(key == 3)//CTRL+C
            {
                inputEventDispatcher.addCase(context, events::io::KeyPressEvent{ events::io::KeyCode::Terminate });
            }
            else if(key == 27)//ESC
            {
                inputEventDispatcher.addCase(context, events::io::KeyPressEvent{ events::io::KeyCode::Escape });
            }
            else if(key == 8)//BACKSPACE
            {
                inputEventDispatcher.addCase(context, events::io::KeyPressEvent{ events::io::KeyCode::Backspace });
            }
            else if(key == 10)//END OF LINE
            {
                inputEventDispatcher.addCase(context, events::io::KeyPressEvent{ events::io::KeyCode::Return });
            }
            else if(key != 0)// Character
            {
                inputEventDispatcher.addCase(context, events::io::KeyPressEvent{ events::io::KeyCode::Character, key });
            }
            else
            {
                inputEventDispatcher.addCase(context, events::io::KeyPressEvent{ events::io::KeyCode::Unknown });
            }
        }

    public:
        explicit ConsoleIODeviceUnix(InputEventDispatcherType& inputEventDispatcher):
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

                //pipe(pipes);

                pollingThread = std::jthread([this, &context](const std::stop_token& stoken)
                {
                    while(!stoken.stop_requested())
                    {
                        if(stoken.stop_requested()) {
                            std::cout << "Sleepy worker is requested to stop\n";
                            return;
                        }

                        int inputKey = getchar();
                        if(!stoken.stop_requested()) {
                            onKeyPressed(context, inputKey);
                        }
                    }
                });

                connected = true;
            }
        }

        void disconnectImpl(TDataContext& context)
        {
            if(connected)
            {
                tcsetattr(0, TCSANOW, &oldt);
                pollingThread.request_stop();
                //close(pipes[1]);
                pollingThread.join();
                connected = false;
            }
        }
    };
}