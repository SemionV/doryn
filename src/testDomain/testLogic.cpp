#include "dependencies.h"
#include "testLogic.h"

namespace test
{
    TestLogic::TestLogic(std::shared_ptr<dory::SystemConsoleEventHub> consoleEventHub):
        consoleEventHub(consoleEventHub)
    {
        consoleEventHub->onKeyPressed() += [] (dory::DataContext& context, dory::KeyPressedEventData& keyPressedEventData)
        {
            if(keyPressedEventData.keyPressed == 27)
            {
                context.isStop = true;
                std::cout << std::this_thread::get_id() << ": ESC" << std::endl;
            }
            else if(keyPressedEventData.keyPressed != 0)
            {
                std::cout << std::this_thread::get_id() << ": key pressed: " << keyPressedEventData.keyPressed << std::endl;
            }
        };
    }
}