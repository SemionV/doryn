#include <dory/core/devices/standardIoDeviceUnix.h>
#include <dory/profiling/profiler.h>

namespace dory::core::devices
{
    StandardIODevice::StandardIODevice(Registry& registry):
            _registry(registry)
    {}

    void StandardIODevice::onKeyPressed(resources::DataContext& context, int key)
    {
        auto inputEventDispatcher = _registry.get<events::io::Bundle::IDispatcher>();
        if(inputEventDispatcher)
        {
            if(key == 3)//CTRL+C
            {
                inputEventDispatcher->charge(events::io::KeyPressEvent{ events::KeyCode::Terminate });
            }
            else if(key == 27)//ESC
            {
                inputEventDispatcher->charge(events::io::KeyPressEvent{ events::KeyCode::Escape });
            }
            else if(key == 127)//BACKSPACE
            {
                inputEventDispatcher->charge(events::io::KeyPressEvent{ events::KeyCode::Backspace });
            }
            else if(key == 10)//END OF LINE
            {
                inputEventDispatcher->charge(events::io::KeyPressEvent{ events::KeyCode::Return });
            }
            else if(key != 0)// Character
            {
                inputEventDispatcher->charge(events::io::KeyPressEvent{ events::KeyCode::Character, key });
            }
            else
            {
                inputEventDispatcher->charge(events::io::KeyPressEvent{ events::KeyCode::Unknown });
            }
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
                profiling::setThreadName("std_io");

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