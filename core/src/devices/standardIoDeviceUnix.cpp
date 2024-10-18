#include <dory/core/devices/standardIoDeviceUnix.h>

namespace dory::core::devices
{
    StandardIODevice::StandardIODevice(std::shared_ptr<events::io::IEventDispatcher> inputEventsDispatcher):
            _inputEventsDispatcher(inputEventsDispatcher)
    {}

    void StandardIODevice::onKeyPressed(resources::DataContext& context, int key)
    {
        if(key == 3)//CTRL+C
        {
            _inputEventsDispatcher->charge(events::io::KeyPressEvent{ events::io::KeyCode::Terminate });
        }
        else if(key == 27)//ESC
        {
            _inputEventsDispatcher->charge(events::io::KeyPressEvent{ events::io::KeyCode::Escape });
        }
        else if(key == 127)//BACKSPACE
        {
            _inputEventsDispatcher->charge(events::io::KeyPressEvent{ events::io::KeyCode::Backspace });
        }
        else if(key == 10)//END OF LINE
        {
            _inputEventsDispatcher->charge(events::io::KeyPressEvent{ events::io::KeyCode::Return });
        }
        else if(key != 0)// Character
        {
            _inputEventsDispatcher->charge(events::io::KeyPressEvent{ events::io::KeyCode::Character, key });
        }
        else
        {
            _inputEventsDispatcher->charge(events::io::KeyPressEvent{ events::io::KeyCode::Unknown });
        }
    }

    void StandardIODevice::out(const std::string& data)
    {
        if(connected)
        {
            std::cout << data;
        }
    }

    void StandardIODevice::flush()
    {
        if(connected)
        {
            std::cout.flush();
        }
    }

    void StandardIODevice::connect(resources::DataContext& context)
    {
        if(!connected)
        {
            tcgetattr(STDIN_FILENO, &oldt);
            currentt = oldt;
            currentt.c_lflag &= ~ICANON; /* disable buffered i/o */
            currentt.c_lflag &= ~ECHO; /* set no echo mode */
            tcsetattr(STDIN_FILENO, TCSANOW, &currentt);

            auto p = pipe(pipes);

            pollingThread = std::jthread([this, &context](const std::stop_token& stoken, void* pipe)
            {
                //see more about this thread unblocking technic here: https://stackoverflow.com/questions/11513593/cancelling-getchar
                int readPipe = *(int*)pipe;
                fd_set rfds;
                int inputKey = 0;

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

    void StandardIODevice::disconnect(resources::DataContext& context)
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
}