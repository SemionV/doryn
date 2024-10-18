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
}